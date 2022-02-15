[**English**](README.md) | **简体中文**

# PointPillars
**高度优化的点云目标检测网络[PointPillars](https://github.com/traveller59/second.pytorch)。主要通过tensorrt对网络推理段进行了优化，通过cuda/c++对前处理后处理进行了优化。做到了真正的实时处理（前处理+后处理小于 1 ms/Head）。**

## Major Advance
- **训练简单**
  
    本仓库直接使用[**mmlab/mmdetection3d**](https://github.com/open-mmlab/mmdetection3d)进行训练。所以只要你按照[**官方教程**](https://mmdetection3d.readthedocs.io/)的教程是非常容易训练自己的数据，也可以直接采用**官方训练参数**来进行部署。但是由于需要使用**TensorRT**,需要对官方版本的网络进行一些改动，我将我的修改版本上传至[*hova88/OpenPCdet*](https://github.com/hova88/OpenPCDet)。

- **部署简单**
   
    本仓库主要是在[**hova88/PointPillars_MultiHead_40FPS**](https://github.com/hova88/PointPillars_MultiHead_40FPS)上做的修改以适配mmdetection3d的模型。


## Requirements (My Environment)
### For *.onnx and *.trt engine file
* Linux Ubuntu 18.04
* OpenPCdet
* ONNX IR version:  0.0.6
* [onnx2trt](https://github.com/onnx/onnx-tensorrt)
  
### For algorithm: 
* Linux Ubuntu 18.04
* CMake 3.17 (版本太低的话cmakelists.txth会找不到cuda)
* CUDA 10.2
* TensorRT 7.1.3 (7以下是不行的)
* yaml-cpp
* google-test (非必须)

### For visualization
* [open3d](https://github.com/intel-isl/Open3D)


## Usage

0. **下载两个工程,并解决环境问题**
   ```bash
   mkdir workspace && cd workspace
   git clone https://github.com/speshowBUAA/PointPillars_mmdet_secfpn.git --recursive && cd ..
   ```


1. **获取 Engine File**

    - 1.1 **Pytorch model --> ONNX model :** 请参考[**speshowBUAA/mmdet3d_onnx_tools**](https://github.com/speshowBUAA/mmdet3d_onnx_tools).

    - 1.2 **ONNX model --> TensorRT model :** 安装[onnx2trt](https://github.com/onnx/onnx-tensorrt)之后，就非常简单。注意，想要加速推理速度，一定要用半精度/混合精度，即（-d 16)
        ```bash
            onnx2trt pts_pfe.onnx -o pts_pfe.trt -b 1 -d 16 
            onnx2trt pts_backbone.onnx -o pts_backbone.trt -b 1 -d 16 
        ```

    - 1.3 **engine file --> algorithm :** 在`bootstrap.yaml`, 指明你生成的两组engine file (*.onnx , *.trt)的路径。 
    - 1.4 下载测试点云[nuscenes_10sweeps_points.txt](https://drive.google.com/file/d/1enCbjwe4giwGC-x7Wjns4eHx2njZW2Jl/view?usp=sharing) ，并在`bootstrap.yaml`指明输入（clouds）与输出(boxes)路径。

2. **编译**

    ```bash
    cd PointPillars_mmdet_secfpn
    mkdir build && cd build
    cmake .. && make -j8 && ./test/test_model
    ```

3. **可视化**

    ```bash
    cd PointPillars_MultiHead_40FPS/tools
    python viewer.py
    ```

# License

GNU General Public License v3.0 or later
See [`COPYING`](LICENSE.md) to see the full text.
