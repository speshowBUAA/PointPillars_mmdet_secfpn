"""
author: hova88
date: 2021/03/16
"""
import numpy as np 
import numpy as np
from visual_tools import draw_clouds_with_boxes, draw_clouds
import open3d as o3d
import yaml

def cfg_from_yaml_file(cfg_file, config):
    with open(cfg_file, 'r') as f:
        try:
            new_config = yaml.load(f, Loader=yaml.FullLoader)
        except:
            new_config = yaml.load(f)

        merge_new_config(config=config, new_config=new_config)

    return config


def dataloader(cloud_path , boxes_path):
    # cloud = np.loadtxt(cloud_path).reshape(-1,5)
    cloud = np.fromfile(cloud_path, dtype=np.float32, count=-1).reshape([-1, 5])[:, :4]
    boxes = np.loadtxt(boxes_path).reshape(-1,7)
    return cloud , boxes 

def main():
    import yaml
    with open("../bootstrap.yaml") as f:
        config = yaml.load(f, Loader=yaml.FullLoader)
    cloud ,boxes = dataloader(config['InputFile'], config['OutputFile'])
    draw_clouds_with_boxes(cloud ,boxes)

def test():
    # path = "../test/testdata/voxel.npy"
    # cloud = np.load(path).reshape(-1, 4)
    # print(cloud.shape)
    # draw_clouds(cloud)

    path = "../test/testdata/np_raw_feats.npy"
    cloud = np.load(path).reshape(-1, 10)
    # cloud = cloud [:, :4]
    print(cloud.shape)
    print(np.max(cloud, axis=0))
    # draw_clouds(cloud)

    path = "../test/testdata/0_Model_pfe_input_gather_feature.txt"
    cloud = np.loadtxt(path).reshape(-1, 10)
    print(np.max(cloud, axis=0))
    # cloud = cloud [:, :4]
    # print(cloud.shape)
    # print(np.max(cloud, axis=0))
    # draw_clouds(cloud)

    # path = "../test/testdata/0_dev_points.txt"
    # cloud = np.loadtxt(path).reshape(-1, 4)
    # print(cloud.shape)
    # print(np.max(cloud, axis=0))
    # draw_clouds(cloud)

    # path = "../test/testdata/0_dev_pfe_gather_feature.txt"
    # cloud = np.loadtxt(path).reshape(-1, 10)
    # cloud = cloud [:, :4]
    # print(cloud.shape)
    # print(np.max(cloud, axis=0))
    # draw_clouds(cloud)

    # path = "../test/testdata/1606813517797756000.bin"
    # cloud = np.fromfile(path, dtype=np.float32, count=-1).reshape([-1, 5])[:, :4]
    # print(cloud.shape)
    # draw_clouds(cloud)

if __name__ == "__main__":
    main()
    # test()