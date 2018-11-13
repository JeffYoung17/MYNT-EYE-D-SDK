// Copyright 2018 Slightech Co., Ltd. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
#include <iostream>

#include <opencv2/highgui/highgui.hpp>

#include "mynteye/camera.h"
#include "mynteye/utils.h"

#include "util/cam_utils.h"
#include "util/counter.h"
#include "util/cv_painter.h"

int main(int argc, char const* argv[]) {
  mynteye::Camera cam;
  mynteye::DeviceInfo dev_info;
  if (!mynteye::util::select(cam, &dev_info)) {
    return 1;
  }
  mynteye::util::print_stream_infos(cam, dev_info.index);

  std::cout << "Open device: " << dev_info.index << ", "
      << dev_info.name << std::endl << std::endl;

  // Warning: Color stream format MJPG doesn't work.
  mynteye::InitParams params(dev_info.index);
  cam.Open(params);

  std::cout << std::endl;
  if (!cam.IsOpened()) {
    std::cerr << "Error: Open camera failed" << std::endl;
    return 1;
  }
  std::cout << "Open device success" << std::endl << std::endl;

  std::cout << "Press ESC/Q on Windows to terminate" << std::endl;

  mynteye::util::Counter counter;
  for (;;) {
    counter.Update();

    auto motion_datas = cam.RetrieveMotions();
    std::cout << "Imu count: " << motion_datas.size() << std::endl;
    if (motion_datas.size() > 0) {
      for (auto data : motion_datas) {
        if (data.imu) {
          if (data.imu->flag == 1) {  // accelerometer
            std::cout << "accel timestamp: " << data.imu->timestamp
              << ", accel_x: " << data.imu->accel[0]
              << ", accel_y: " << data.imu->accel[1]
              << ", accel_z: " << data.imu->accel[2]
              << ", temperature: " << data.imu->temperature
              << std::endl;
          } else if (data.imu->flag == 2) {  // gyroscope
            std::cout << "gyro timestamp: " << data.imu->timestamp
              << ", gyro_x: " << data.imu->gyro[0]
              << ", gyro_y: " << data.imu->gyro[1]
              << ", gyro_z: " << data.imu->gyro[2]
              << ", temperature: " << data.imu->temperature
              << std::endl;
          } else {
            std::cerr << "Imu type is unknown" << std::endl;
          }
        } else {
          std::cerr << "Motion data is empty" << std::endl;
        }
      }
      std::cout << std::endl;
    }

#ifdef MYNTEYE_OS_LINUX
    char key = mynteye::util::waitKey();
    if (key == 27 || key == 'q' || key == 'Q') {  // ESC/Q
      break;
    }
#endif
    cam.Wait();  // keep frequency
  }

  cam.Close();
  return 0;
}
