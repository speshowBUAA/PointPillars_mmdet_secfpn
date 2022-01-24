/******************************************************************************
 * Copyright 2020 The Apollo Authors. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *****************************************************************************/

/*
 * Copyright 2018-2019 Autoware Foundation. All rights reserved.
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

/**
 * @author Kosuke Murakami
 * @date 2019/02/26
 */

/**
* @author Yan haixu
* Contact: just github.com/hova88
* @date 2021/04/30
*/

/**
* @author Ye xiubo
* Contact:github.com/speshowBUAA
* @date 2022/01/05
*/

#pragma once

// headers in STL
#include <algorithm>
#include <cmath>
#include <iomanip>
#include <limits>
#include <map>
#include <memory>
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <fstream>
#include <assert.h>
// headers in TensorRT
#include "NvInfer.h"
#include "NvOnnxParser.h"
#include "NvInferPlugin.h"

// headers in local files
// #include "params.h"
#include "common.h"
#include <yaml-cpp/yaml.h>
#include "preprocess.h"
#include "scatter.h"
#include "postprocess.h"

using namespace std;

// Logger for TensorRT info/warning/errors
class Logger : public nvinfer1::ILogger {
 public:
  explicit Logger(Severity severity = Severity::kWARNING)
      : reportable_severity(severity) {}

  void log(Severity severity, const char* msg) noexcept override {
    // suppress messages with severity enum value greater than the reportable
    if (severity > reportable_severity) return;

    switch (severity) {
      case Severity::kINTERNAL_ERROR:
        std::cerr << "INTERNAL_ERROR: ";
        break;
      case Severity::kERROR:
        std::cerr << "ERROR: ";
        break;
      case Severity::kWARNING:
        std::cerr << "WARNING: ";
        break;
      case Severity::kINFO:
        std::cerr << "INFO: ";
        break;
      default:
        std::cerr << "UNKNOWN: ";
        break;
    }
    std::cerr << msg << std::endl;
  }

  Severity reportable_severity;
};



class PointPillars {
 private:
    // initialize in initializer list
    const float score_threshold_;
    const float nms_overlap_threshold_;
    const bool use_onnx_;
    const std::string pfe_file_;
    const std::string backbone_file_;
    const std::string pp_config_;
    // end initializer list
    // voxel size
    float kPillarXSize;
    float kPillarYSize;
    float kPillarZSize;
    // point cloud range
    float kMinXRange;
    float kMinYRange;
    float kMinZRange;
    float kMaxXRange;
    float kMaxYRange;
    float kMaxZRange;
    // hyper parameters
    int kNumClass;
    int kMaxNumPillars;
    int kMaxNumPointsPerPillar;
    int kNumPointFeature;
    int kNumAnchorSize;
    // int kNumGatherPointFeature = 11;    // multihead_pp 是11
    int kNumGatherPointFeature = 10;      // mmdet3d 是10
    int kGridXSize;
    int kGridYSize;
    int kGridZSize;
    int kRpnInputSize;
    int kNumAnchor;
    int kNumInputBoxFeature;
    int kNumOutputBoxFeature;
    int kBatchSize;
    int kNumIndsForScan;
    int kNumThreads;
    // if you change kNumThreads, need to modify NUM_THREADS_MACRO in
    // common.h
    int kNumBoxCorners;
    int kNmsPreMaxsize;
    int kNmsPostMaxsize;

    int* coor_to_voxelidx;
    int voxel_num_;

    int host_pillar_count_[1];

    int* dev_x_coors_;
    int* dev_y_coors_;
    float* dev_num_points_per_pillar_;
    int* dev_sparse_pillar_map_;
    int* dev_cumsum_along_x_;
    int* dev_cumsum_along_y_;

    float* dev_pillar_point_feature_;
    float* dev_pillar_coors_;
    float* dev_points_mean_;

    float* dev_pfe_gather_feature_;
    void* pfe_buffers_[2];
    void* rpn_buffers_[4];

    float* dev_scattered_feature_;

    float* host_box_;
    float* host_score_;
    int*   host_filtered_count_;

    std::unique_ptr<PreprocessPointsCuda> preprocess_points_cuda_ptr_;
    std::unique_ptr<ScatterCuda> scatter_cuda_ptr_;
    std::unique_ptr<PostprocessCuda> postprocess_cuda_ptr_;

    Logger g_logger_;
    nvinfer1::ICudaEngine* pfe_engine_;
    nvinfer1::ICudaEngine* backbone_engine_;
    nvinfer1::IExecutionContext* pfe_context_;
    nvinfer1::IExecutionContext* backbone_context_;

    /**
     * @brief Memory allocation for device memory
     * @details Called in the constructor
     */
    void DeviceMemoryMalloc();

    /**
     * @brief Memory set to 0 for device memory
     * @details Called in the DoInference
     */
    void SetDeviceMemoryToZero();

    /**
     * @brief Initializing paraments from pointpillars.yaml
     * @details Called in the constructor
     */
    void InitParams();
    /**
     * @brief Initializing TensorRT instances
     * @param[in] usr_onnx_ if true, parse ONNX 
     * @details Called in the constructor
     */
    void InitTRT(const bool use_onnx);
    /**
     * @brief Convert ONNX to TensorRT model
     * @param[in] model_file ONNX model file path
     * @param[out] engine_ptr TensorRT model engine made out of ONNX model
     * @details Load ONNX model, and convert it to TensorRT model
     */
    void OnnxToTRTModel(const std::string& model_file,
                        nvinfer1::ICudaEngine** engine_ptr);

    /**
     * @brief Convert Engine to TensorRT model
     * @param[in] model_file Engine(TensorRT) model file path
     * @param[out] engine_ptr TensorRT model engine made 
     * @details Load Engine model, and convert it to TensorRT model
     */
    void EngineToTRTModel(const std::string &engine_file ,     
                        nvinfer1::ICudaEngine** engine_ptr) ;

    /**
     * @brief Preproces points
     * @param[in] in_points_array Point cloud array
     * @param[in] in_num_points Number of points
     * @details Call CPU or GPU preprocess
     */
    void Preprocess(const float* in_points_array, const int in_num_points);

    public:
    /**
     * @brief Constructor
     * @param[in] score_threshold Score threshold for filtering output
     * @param[in] nms_overlap_threshold IOU threshold for NMS
     * @param[in] use_onnx if true,using onnx file ,else using engine file
     * @param[in] pfe_file Pillar Feature Extractor ONNX file path
     * @param[in] rpn_file Region Proposal Network ONNX file path
     * @details Variables could be changed through point_pillars_detection
     */
    PointPillars(const float score_threshold,
                const float nms_overlap_threshold,
                const bool use_onnx,
                const std::string pfe_file,
                const std::string rpn_file,
                const std::string pp_config);
    ~PointPillars();

    /**
     * @brief Call PointPillars for the inference
     * @param[in] in_points_array Point cloud array
     * @param[in] in_num_points Number of points
     * @param[out] out_detections Network output bounding box
     * @param[out] out_labels Network output object's label
     * @details This is an interface for the algorithm
     */
    void DoInference(const float* in_points_array,
                    const int in_num_points,
                    const float* in_anchor_points,
                    std::vector<float>* out_detections,
                    std::vector<int>* out_labels,
                    std::vector<float>* out_scores);
};

