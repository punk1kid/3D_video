#include "../src/mp4parser.h"
#include "../src/source.h"

using namespace mp4parser;
using namespace diplomproject;

int main() {
  setlocale(0, "RU");
  std::string path1 = "resources\\video\\2.mp4";
  std::string path2 = "";
  Mp4Data mp4(path1);

  if (mp4.GetError() == Mp4Data::err_path) {
    std::cout << "error path!";
  }
  else {
    auto wh = mp4.GetWH();
    auto duration = mp4.GetVideoDuration();
    auto timescale = mp4.GetTimescale();
    auto fps = mp4.GetFPS();
    auto sample_count = mp4.GetSampleCount();

    uint32_t s = mp4.GetTimescale();
  }

  return 0;
}