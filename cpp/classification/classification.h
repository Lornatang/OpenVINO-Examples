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
#ifndef CLASSIFICATION
#define CLASSIFICATION

#include <condition_variable>
#include <fstream>
#include <iostream>
#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <vector>

#include "gflags/gflags.h"
#include "opencv2/opencv.hpp"
#include "openvino/format_reader/format_reader_ptr.h"
#include "openvino/ie_core.hpp"
#include "openvino/inference_engine.hpp"
#include "openvino/samples/args_helper.hpp"
#include "openvino/samples/classification_results.h"
#include "openvino/samples/ocv_common.hpp"
#include "openvino/samples/slog.hpp"

// Define all help message.
static const char help_message[] = "Print a usage message.";
static const char image_message[] = "Required. Path to a folder with images or path to an image files: a .ubyte file for LeNet and a .bmp file for the other networks.";
static const char model_message[] = "Required. Path to an .xml file with a trained model.";
static const char label_message[] = "Required. Path to an .txt file with a trained model.";
static const char target_device_message[] = "Optional. Specify the target device to infer on (the list of available devices is shown below). Default value is CPU. Sample will look for a suitable plugin for device specified.";
static const char ntop_message[] = "Optional. Number of top results. Default value is 10.";
static const char custom_cldnn_message[] = "Required for GPU custom kernels. Absolute path to the .xml file with kernels description";
static const char custom_cpu_library_message[] = "Required for CPU custom layers. Absolute path to a shared library with the kernels implementation";

// Define all params.
DEFINE_bool(h, false, help_message);
DEFINE_string(i, "", image_message);
DEFINE_string(model, "", model_message);
DEFINE_string(labels, "", label_message);
DEFINE_string(device, "CPU", target_device_message);
DEFINE_uint32(ntop, 10, ntop_message);
DEFINE_string(c, "", custom_cldnn_message);
DEFINE_string(l, "", custom_cpu_library_message);

/**
 * @brief This function is parse params.
 */
bool parse_and_check_command_line(int argc, char **argv);

#endif  // CLASSIFICATION