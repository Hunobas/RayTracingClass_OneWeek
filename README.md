# Ray Tracing in One Weekend - C++ 구현

<img width="1200" height="800" alt="다운로드 (12)" src="https://github.com/user-attachments/assets/4301dd4e-b67d-461b-9449-7d4beea3c24b" />

## 개요

Peter Shirley의 [Ray Tracing in One Weekend](https://raytracing.github.io/books/RayTracingInOneWeekend.html)를 따라 C++로 레이 트레이서를 만들었습니다.

강의 내용을 그대로 따라가는 데서 그치지 않고, 안티앨리어싱 과정에서 `std::future`를 써서 병렬 처리를 붙여봤습니다. 렌더링 시간이 79분에서 18분으로 줄었습니다.

## 구현 내용

- 광선 생성 및 물체 교차 판정
- Lambertian, Metal, Dielectric 재질
- Defocus Blur (피사계 심도)
- 안티앨리어싱 (픽셀당 다중 샘플링)
- 멀티스레딩 최적화

## 멀티스레딩 적용

원본 코드는 단일 스레드입니다. 픽셀 하나당 수백 번 샘플링을 돌리다 보니 시간이 오래 걸렸습니다.

해결 방법으로 **픽셀당 샘플 수를 스레드 개수로 나눠서** 각 스레드가 일부 샘플만 담당하게 했습니다. 람다 함수로 작업 단위를 정의하고, `std::async`로 비동기 실행한 뒤 `std::future`로 결과를 모읍니다.

```cpp
// 스레드 설정
const unsigned n_threads = N_THREADS;
std::vector<std::future<color>> futures;
futures.resize(n_threads);
const unsigned n_per_thread = samples_per_pixel / n_threads;

// 각 스레드가 담당할 샘플 범위를 나눠서 처리
auto make_scene = [&](const int& i, const int& j, const int& t) {
    color pixel_color(0, 0, 0);
    int s_start = t * n_per_thread;
    int s_end = (t + 1) * n_per_thread;
    for (unsigned s = s_start; s < s_end; ++s)
    {
        double u = double(i) / (image_width - 1);
        double v = double(j) / (image_height - 1);
        ray r = cam.get_ray(u, v);
        pixel_color += ray_color(r, world, max_depth);
    }
    return pixel_color;
};

// 렌더링 루프
for (int i = 0; i < image_width; ++i)
{
    color pixel_color(0, 0, 0);
    for (unsigned t = 0; t < n_threads; ++t)
        futures[t] = std::async(make_scene, i, j, t);
    for (unsigned t = 0; t < n_threads; ++t)
        pixel_color += futures[t].get();
    
    write_color(ppm1, j, i, pixel_color, samples_per_pixel);
}
```

### 결과

| 항목 | 단일 스레드 | 멀티스레드 |
|------|-------------|------------|
| 렌더링 시간 | 79분 | 18분 |

약 77% 단축. 재귀 깊이 50, 샘플 수 500 기준.

## 배운 점

- 광선 추적의 기본 (반사, 굴절, 산란)
- 벡터 수학
- C++ 비동기 프로그래밍 (`std::async`, `std::future`)

## 빌드

Visual Studio 솔루션 파일 사용. Release 모드 권장.

출력되는 PPM 파일은 이미지 뷰어나 Photoshop에서 열 수 있습니다.

## 참고

- [Ray Tracing in One Weekend](https://raytracing.github.io/books/RayTracingInOneWeekend.html) - Peter Shirley
- [std::future - cppreference](https://en.cppreference.com/w/cpp/thread/future)
