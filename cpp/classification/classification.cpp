/*
 * Copyright (c) 2020 Dakewe Biotech Corporation. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

// This is a classification example.

#include "classification.h"

/**
 * @brief This function show a help message.
 */
static void show_usage() {
  std::cout << std::endl;
  std::cout << "classification_sample_async [OPTION]" << std::endl;
  std::cout << "Options:" << std::endl;
  std::cout << std::endl;
  std::cout << "    -h                      " << help_message << std::endl;
  std::cout << "    -i <path>               " << image_message << std::endl;
  std::cout << "    -model <path>           " << model_message << std::endl;
  std::cout << "    -labels <path>          " << label_message << std::endl;
  std::cout << "      -l <absolute_path>    " << custom_cpu_library_message << std::endl;
  std::cout << "          Or" << std::endl;
  std::cout << "      -c <absolute_path>    " << custom_cldnn_message << std::endl;
  std::cout << "    -device <device>        " << target_device_message << std::endl;
  std::cout << "    -ntop <integer>         " << ntop_message << std::endl;
}

/**
 * @brief This function is parse params.
 */
bool parse_and_check_command_line(int argc, char **argv) {
  // ---------------------------Parsing and validation of input args--------------------------------------
  slog::info << "Parsing input parameters" << slog::endl;

  gflags::ParseCommandLineNonHelpFlags(&argc, &argv, true);
  if (FLAGS_h) {
    show_usage();
    showAvailableDevices();
    return false;
  }
  slog::info << "Parsing input parameters" << slog::endl;

  if (FLAGS_i.empty()) {
    throw std::logic_error("Parameter -i is not set");
  }

  if (FLAGS_model.empty()) {
    throw std::logic_error("Parameter -model is not set");
  }

  if (FLAGS_labels.empty()) {
    throw std::logic_error("Parameter -labels is not set");
  }

  return true;
}

int main(int argc, char *argv[]) {
  try {
    slog::info << "InferenceEngine: " << InferenceEngine::GetInferenceEngineVersion() << slog::endl;

    // ------------------------------ Parsing and validation of input args ---------------------------------
    if (!parse_and_check_command_line(argc, argv)) {
      return 0;
    }

    /** This vector stores paths to the processed images **/
    std::vector<std::string> image_names;
    parseInputFilesArguments(image_names);
    if (image_names.empty()) throw std::logic_error("No suitable images were found");
    // -----------------------------------------------------------------------------------------------------

    // --------------------------- 1. Load inference engine -------------------------------------
    slog::info << "Creating Inference Engine" << slog::endl;

    InferenceEngine::Core ie;

    if (!FLAGS_l.empty()) {
      // CPU(MKLDNN) extensions are loaded as a shared library and passed as a pointer to base extension
      InferenceEngine::IExtensionPtr extension_ptr = InferenceEngine::make_so_pointer<InferenceEngine::IExtension>(FLAGS_l);
      ie.AddExtension(extension_ptr);
      slog::info << "CPU Extension loaded: " << FLAGS_l << slog::endl;
    }
    if (!FLAGS_c.empty()) {
      // clDNN Extensions are loaded from an .xml description and OpenCL kernel files
      ie.SetConfig({{InferenceEngine::PluginConfigParams::KEY_CONFIG_FILE, FLAGS_c}}, "GPU");
      slog::info << "GPU Extension loaded: " << FLAGS_c << slog::endl;
    }

    /** Printing device version **/
    std::cout << ie.GetVersions(FLAGS_device) << std::endl;
    // -----------------------------------------------------------------------------------------------------

    // 2. Read a model in OpenVINO Intermediate Representation (.xml and .bin files) or ONNX (.onnx file) format
    slog::info << "Loading network files" << slog::endl;

    /** Read network model **/
    InferenceEngine::CNNNetwork network = ie.ReadNetwork(FLAGS_model);
    // -----------------------------------------------------------------------------------------------------

    // --------------------------- 3. Configure input & output ---------------------------------------------

    // --------------------------- Prepare input blobs -----------------------------------------------------
    slog::info << "Preparing input blobs" << slog::endl;

    /** Taking information about all topology inputs **/
    InferenceEngine::InputsDataMap input_info(network.getInputsInfo());
    if (input_info.size() != 1) throw std::logic_error("Sample supports topologies with 1 input only");

    auto input_info_item = *input_info.begin();

    /** Specifying the precision and layout of input data provided by the user.
     * This should be called before load of the network to the device **/
    input_info_item.second->setPrecision(InferenceEngine::Precision::U8);
    input_info_item.second->setLayout(InferenceEngine::Layout::NCHW);

    std::vector<std::shared_ptr<unsigned char>> image_data = {};
    std::vector<std::string> valid_image_name = {};
    for (const auto &i : image_names) {
      FormatReader::ReaderPtr reader(i.c_str());
      if (reader.get() == nullptr) {
        slog::warn << "Image " + i + " cannot be read!" << slog::endl;
        continue;
      }
      /** Store image data **/
      std::shared_ptr<unsigned char> data(reader->getData(input_info_item.second->getTensorDesc().getDims()[3],
                                                          input_info_item.second->getTensorDesc().getDims()[2]));
      if (data != nullptr) {
        image_data.push_back(data);
        valid_image_name.push_back(i);
      }
    }
    if (image_data.empty()) throw std::logic_error("Valid input images were not found!");

    /** Setting batch size using image count **/
    network.setBatchSize(image_data.size());
    size_t batch_size = network.getBatchSize();
    slog::info << "Batch size is " << std::to_string(batch_size) << slog::endl;

    // -----------------------------------------------------------------------------------------------------

    // --------------------------- 4. Loading model to the device ------------------------------------------
    slog::info << "Loading model to the device" << slog::endl;
    InferenceEngine::ExecutableNetwork executable_network = ie.LoadNetwork(network, FLAGS_device);
    // -----------------------------------------------------------------------------------------------------

    // --------------------------- 5. Create infer request -------------------------------------------------
    slog::info << "Create inference request" << slog::endl;
    InferenceEngine::InferRequest inference_request = executable_network.CreateInferRequest();
    // -----------------------------------------------------------------------------------------------------

    // --------------------------- 6. Prepare input --------------------------------------------------------
    for (auto &item : input_info) {
      InferenceEngine::Blob::Ptr input_blob = inference_request.GetBlob(item.first);
      InferenceEngine::SizeVector dims = input_blob->getTensorDesc().getDims();
      /** Fill input tensor with images. First b channel, then g and r channels **/
      size_t num_channels = dims[1];
      size_t image_size = dims[3] * dims[2];

      InferenceEngine::MemoryBlob::Ptr minput = as<InferenceEngine::MemoryBlob>(input_blob);
      if (!minput) {
        slog::err << "We expect MemoryBlob from inferRequest, but by fact we were not able to cast inputBlob to MemoryBlob" << slog::endl;
        return 1;
      }
      // locked memory holder should be alive all time while access to its buffer happens
      auto minputHolder = minput->wmap();

      auto data = minputHolder.as<InferenceEngine::PrecisionTrait<InferenceEngine::Precision::U8>::value_type *>();
      // Iterate over all input images
      for (size_t image_id = 0; image_id < image_data.size(); ++image_id) {
        // Iterate over all pixel in image (b,g,r)
        for (size_t pid = 0; pid < image_size; pid++) {
          // Iterate over all channels
          for (size_t ch = 0; ch < num_channels; ++ch) {
            // [images stride + channels stride + pixel id ] all in bytes
            data[image_id * image_size * num_channels + ch * image_size + pid] = image_data.at(image_id).get()[pid * num_channels + ch];
          }
        }
      }
    }

    // -----------------------------------------------------------------------------------------------------

    // --------------------------- 7. Do inference ---------------------------------------------------------
    size_t num_iterations = 10;
    size_t current_iteration = 0;
    std::condition_variable condVar;

    inference_request.SetCompletionCallback([&] {
        current_iteration++;
        slog::info << "Completed " << current_iteration << " async request execution" << slog::endl;
        if (current_iteration < num_iterations) {
          /* here a user can read output containing inference results and put new input
             to repeat async request again */
          inference_request.StartAsync();
        } else {
          /* continue sample execution after last Asynchronous inference request execution */
          condVar.notify_one();
        }
    });

    /* Start async request for the first time */
    slog::info << "Start inference (" << num_iterations << " asynchronous executions)" << slog::endl;
    inference_request.StartAsync();

    /* Wait all repetitions of the async request */
    std::mutex mutex;
    std::unique_lock<std::mutex> lock(mutex);
    condVar.wait(lock, [&] { return current_iteration == num_iterations; });

    // -----------------------------------------------------------------------------------------------------

    // --------------------------- 8. Process output -------------------------------------------------------
    slog::info << "Processing output blobs" << slog::endl;
    InferenceEngine::OutputsDataMap output_info(network.getOutputsInfo());
    if (output_info.size() != 1) throw std::logic_error("Sample supports topologies with 1 output only");
    InferenceEngine::Blob::Ptr output_blob = inference_request.GetBlob(output_info.begin()->first);

    /** Validating -nt value **/
    const size_t results_count = output_blob->size() / batch_size;
    if (FLAGS_ntop > results_count || FLAGS_ntop < 1) {
      slog::warn << "-nt " << FLAGS_ntop << " is not available for this network (-nt should be less than " << results_count + 1
                 << " and more than 0)\n            will be used maximal value : " << results_count << slog::endl;
      FLAGS_ntop = results_count;
    }

    /** Read labels from file (e.x. AlexNet.labels) **/
    std::string label_file_name = fileNameNoExt(FLAGS_labels);
    std::vector<std::string> labels;

    std::ifstream input_file;
    input_file.open(label_file_name, std::ios::in);
    if (input_file.is_open()) {
      std::string str_line;
      while (std::getline(input_file, str_line)) {
        trim(str_line);
        labels.push_back(str_line);
      }
    }

    ClassificationResult classification_result(output_blob, valid_image_name, batch_size, FLAGS_ntop, labels);
    classification_result.print();
    // -----------------------------------------------------------------------------------------------------
  }
  catch (const std::exception &error) {
    slog::err << error.what() << slog::endl;
    return 1;
  }
  catch (...) {
    slog::err << "Unknown/internal exception happened." << slog::endl;
    return 1;
  }

  slog::info << "Execution successful" << slog::endl;
  return 0;
}
