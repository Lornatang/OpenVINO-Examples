### Classifier

#### Requirement

- Data
```shell script
# dog
$ wget https://pic2.zhimg.com/v2-e861d4e35bd63868a5f3af65987430df_r.jpg?source=1940ef5c -O dog.jpg
# cat 
$ wget https://gss0.baidu.com/94o3dSag_xI4khGko9WTAnF6hhy/zhidao/wh%3D600%2C800/sign=af1c83c6952f07085f502206d91494a1/0ff41bd5ad6eddc47910257232dbb6fd53663347.jpg -O cat.jpg
# car
$ wget https://s3.caradvice.com.au/wp-content/uploads/2016/06/2016-nissan_gtr-nissan_gt_r-17.jpg -O car.jpg
```

- Model
```shell script
# alexnet
$ python3 /opt/intel/openvino/deployment_tools/open_model_zoo/tools/downloader/downloader.py --name alexnet
# vgg16
$ python3 /opt/intel/openvino/deployment_tools/open_model_zoo/tools/downloader/downloader.py --name vgg16
# vgg19
$ python3 /opt/intel/openvino/deployment_tools/open_model_zoo/tools/downloader/downloader.py --name vgg19
# squeezenet1.1
$ python3 /opt/intel/openvino/deployment_tools/open_model_zoo/tools/downloader/downloader.py --name squeezenet1.1
# mobilenet-v1
$ python3 /opt/intel/openvino/deployment_tools/open_model_zoo/tools/downloader/downloader.py --name mobilenet-v1-1.0-224
# Show all model
$ python3 /opt/intel/openvino/deployment_tools/open_model_zoo/tools/downloader/downloader.py --print_all 
```

#### Run

- Convert model
```shell script
# mobilenet-v1
$ python3 /opt/intel/openvino/deployment_tools/model_optimizer/mo.py --input_model public/mobilenet-v1-1.0-224/mobilenet-v1-1.0-224.caffemodel --input_proto public/mobilenet-v1-1.0-224/mobilenet-v1-1.0-224.prototxt
```

- Test
```shell script
# Build example
$ mkdir build
$ cd build
$ cmake .. && make -j8
# Run example
$ ./classification -i ../dog.jpg -model ../mobilenet-v1-1.0-224.xml -labels ../../../assets/imagenet.labels -device CPU
```
