
**English** | [**简体中文**](README_zh-CN.md)

# PointPillars
**High performance version of 3D object detection network -[PointPillars](https://github.com/traveller59/second.pytorch), which can achieve the real-time processing (less than 1 ms / head)**
1. The inference part of **PointPillars**(pfe , backbone(multihead)) is optimized by tensorrt
2. The pre- and post- processing are optimized by CUDA / C + recode.

## Major Advance
- **Easy to train**
  
    - this repo directly uses [**mmlab/mmdetection3d**](https://github.com/open-mmlab/mmdetection3d) for training. Therefore, as long as you follow the steps of the [**official tutorial**](https://mmdetection3d.readthedocs.io/), it is very easy to train your own data. In addition, you can directly use [**official weight(hv_pointpillars_secfpn_sbn-all_4x8_2x_nus-3d)**](https://download.openmmlab.com/mmdetection3d/v0.1.0_models/pointpillars/hv_pointpillars_secfpn_sbn-all_4x8_2x_nus-3d/hv_pointpillars_secfpn_sbn-all_4x8_2x_nus-3d_20200620_230725-0817d270.pth) for deployment. Due to the need to use **Tensorrt**, the official **mmlab/mmdetection3d** still needs to be customized. You can find onnx export tool in [**speshowBUAA/mmdet3d_onnx_tools**](https://github.com/speshowBUAA/mmdet3d_onnx_tools).


- **Easy to deploy**
    - this repo is mainly modified to be compatible with mmdet3d models on the basis of [**hova88/PointPillars_MultiHead_40FPS**](https://github.com/hova88/PointPillars_MultiHead_40FPS)

## Requirements (My Environment)
### For *.onnx and *.trt engine file
* Linux Ubuntu 18.04
* mmdetection3d
* ONNX IR version:  0.0.6
* [onnx2trt](https://github.com/onnx/onnx-tensorrt)
  
### For algorithm: 
* Linux Ubuntu 18.04
* CMake 3.17 
* CUDA 10.2
* TensorRT 7.1.3 
* yaml-cpp
* google-test (not necessary)

### For visualization
* [open3d](https://github.com/intel-isl/Open3D)


## Usage

1. **clone thest two repositories, and make sure the dependences is complete**
   ```bash
   mkdir workspace && cd workspace
   git clone https://github.com/speshowBUAA/PointPillars_mmdet_secfpn.git --recursive && cd ..
   ```

2. **generate engine file**

    - 1.1 **Pytorch model --> ONNX model :** Please refer to [**speshowBUAA/mmdet3d_onnx_tools**](https://github.com/speshowBUAA/mmdet3d_onnx_tools).

    - 1.2 **ONNX model --> TensorRT model :** after install the [onnx2trt](https://github.com/onnx/onnx-tensorrt), things become very simple. Note that if you want to further improve the the inference speed, you must use half precision or mixed precision(like ,-d 16)
        ```bash
            onnx2trt pts_pfe.onnx -o pts_pfe.trt -b 1 -d 16 
            onnx2trt pts_backbone.onnx -o pts_backbone.trt -b 1 -d 16 
        ```

    - 1.3 **engine file --> algorithm :** Specified the path of engine files(*.onnx , *.trt) in`bootstrap.yaml`.
  
    - 1.4 Download the test pointcloud [nuscenes_10sweeps_points.txt](https://drive.google.com/file/d/1KD0LT0kzcpGUysUu__dfnfYnHUW62iwN/view?usp=sharing), and specified the path in `bootstrap.yaml`.

3. **Compiler**

    ```bash
    cd PointPillars_mmdet_secfpn
    mkdir build && cd build
    cmake .. && make -j8 && ./test/test_model
    ```

4. **Visualization**

    ```bash
    cd PointPillars_mmdet_secfpn/tools
    python viewer.py
    ```

GNU General Public License v3.0 or later
See [`COPYING`](LICENSE.md) to see the full text.
