#include <d3d11.h>
#include <stdio.h>
#include <dxgi.h>
#include <d2d1.h>
#define WIDTH 200

int buf[WIDTH*200];

void
delupus(void *b, size_t length)
{
  int sum = 0;
  char *buf = (char*)b;
  char *end = buf + length;
  while (buf < end) {
    sum += *buf;
    buf += 0x1000;
  }
  printf("sum: %d\n", sum);
}

double ticksToMS;
int main(int argc, char **argv)
{
  int size;
  ID3D11Device *device;
  ID3D11DeviceContext *context;
  D3D_FEATURE_LEVEL level;
  D3D11CreateDevice(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0,
                    NULL, 0, D3D11_SDK_VERSION, &device,
                    &level, &context);
  printf("%p\n", device);
  if (argc > 1)
    size = strtol(argv[1], 0, NULL);
  else
    size = D3D11_REQ_TEXTURE2D_U_OR_V_DIMENSION;
  int size_low = 0;
  int trial_size;
  HRESULT result;
      
    LARGE_INTEGER freq;
    QueryPerformanceFrequency(&freq);
    printf("frequency %lld\n", freq.QuadPart);
    ticksToMS = 1000.*1000./freq.QuadPart;
    trial_size = size;
    result = 0x800000;
    D3D11_TEXTURE2D_DESC desc;
    desc.Width = WIDTH;
    desc.Height = 200;
    desc.MipLevels = 1;
    desc.ArraySize = 1;
    desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.SampleDesc.Count = 1;
    desc.SampleDesc.Quality = 0;
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.CPUAccessFlags = 0;
    desc.MiscFlags = 0;
    //desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    desc.BindFlags = D3D11_BIND_RENDER_TARGET;
    {
      ID3D11Texture2D *texture;
      LARGE_INTEGER before;
      LARGE_INTEGER after;
      QueryPerformanceCounter(&before);
      result = device->CreateTexture2D(&desc, NULL, &texture);
      QueryPerformanceCounter(&after);
      LONGLONG time = after.QuadPart - before.QuadPart;
      printf("%x naked time: %.1fus\n", result, time*ticksToMS);
    }
    {
      ID3D11Texture2D *texture;
      D3D11_SUBRESOURCE_DATA data;
      data.pSysMem = buf;
      data.SysMemPitch = WIDTH*4;
      data.SysMemSlicePitch = 0;
      LARGE_INTEGER before;
      LARGE_INTEGER after;
      QueryPerformanceCounter(&before);
      result = device->CreateTexture2D(&desc, &data, &texture);
      QueryPerformanceCounter(&after);
      LONGLONG time = after.QuadPart - before.QuadPart;
      printf("%x fill time: %.1fus\n", result, time*ticksToMS);
    }
    {
      ID3D11Texture2D *texture;
      LARGE_INTEGER before;
      LARGE_INTEGER after;
      QueryPerformanceCounter(&before);
      result = device->CreateTexture2D(&desc, NULL, &texture);
      QueryPerformanceCounter(&after);
      LONGLONG time = after.QuadPart - before.QuadPart;
      printf("%x naked time: %.1fus\n", result, time*ticksToMS);
    }
    {
      ID3D11Texture2D *texture;
      D3D11_SUBRESOURCE_DATA data;
      data.pSysMem = buf;
      data.SysMemPitch = WIDTH*4;
      data.SysMemSlicePitch = 0;
      LARGE_INTEGER before;
      LARGE_INTEGER after;
      QueryPerformanceCounter(&before);
      result = device->CreateTexture2D(&desc, &data, &texture);
      QueryPerformanceCounter(&after);
      LONGLONG time = after.QuadPart - before.QuadPart;
      printf("%x fill time: %.1fus\n", result, time*ticksToMS);
    }

    {
      ID3D11Texture2D *destTexture;
      D3D11_MAPPED_SUBRESOURCE map;
      LARGE_INTEGER before;
      LARGE_INTEGER after;
      QueryPerformanceCounter(&before);

      desc.BindFlags = D3D11_BIND_RENDER_TARGET; 
      result = device->CreateTexture2D(&desc, NULL, &destTexture);
      QueryPerformanceCounter(&after);
      LONGLONG time = after.QuadPart - before.QuadPart;
      printf("%x naked time: %.1fus\n", result, time*ticksToMS);

      desc.Usage = D3D11_USAGE_STAGING;
      desc.BindFlags = 0;
      desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;
      ID3D11Texture2D *texture;
      QueryPerformanceCounter(&before);
      result = device->CreateTexture2D(&desc, NULL, &texture);
      QueryPerformanceCounter(&after);
      time = after.QuadPart - before.QuadPart;
      printf("%x staging time: %.1fus\n", result, time*ticksToMS);

      QueryPerformanceCounter(&before);
      result = context->Map(texture, 0, D3D11_MAP_READ_WRITE, 0, &map);
      QueryPerformanceCounter(&after);
      time = after.QuadPart - before.QuadPart;
      printf("%x staging map time: %.1fus\n", result, time*ticksToMS);

      //delupus(map.pData, 200*WIDTH*4);
      QueryPerformanceCounter(&before);
      memcpy(map.pData, buf, 200*WIDTH*4);
      QueryPerformanceCounter(&after);
      time = after.QuadPart - before.QuadPart;
      printf("%x memcpy time: %.1fus\n", result, time*ticksToMS);


      QueryPerformanceCounter(&before);
      context->Unmap(texture, 0);
      QueryPerformanceCounter(&after);
      time = after.QuadPart - before.QuadPart;
      printf("%x staging unmap time: %.1fus\n", result, time*ticksToMS);

      QueryPerformanceCounter(&before);
      context->CopyResource(texture, destTexture);
      QueryPerformanceCounter(&after);
      time = after.QuadPart - before.QuadPart;
      printf("%x copy time: %.1fus\n", result, time*ticksToMS);

      QueryPerformanceCounter(&before);
      result = context->Map(texture, 0, D3D11_MAP_READ_WRITE, 0, &map);
      QueryPerformanceCounter(&after);
      time = after.QuadPart - before.QuadPart;
      printf("%x staging map time: %.1fus\n", result, time*ticksToMS);

      QueryPerformanceCounter(&before);
      memcpy(map.pData, buf, 200*WIDTH*4);
      QueryPerformanceCounter(&after);
      time = after.QuadPart - before.QuadPart;
      printf("%x memcpy time: %.1fus\n", result, time*ticksToMS);


      QueryPerformanceCounter(&before);
      context->Unmap(texture, 0);
      QueryPerformanceCounter(&after);
      time = after.QuadPart - before.QuadPart;
      printf("%x staging unmap time: %.1fus\n", result, time*ticksToMS);

      QueryPerformanceCounter(&before);
      context->CopyResource(destTexture, texture);
      QueryPerformanceCounter(&after);
      time = after.QuadPart - before.QuadPart;
      printf("%x copy time: %.1fus\n", result, time*ticksToMS);

      // Try to create a shared bitmap
      IDXGISurface1* surface;
      ID2D1RenderTarget* renderTarget;
      HRESULT hr;
      hr = destTexture->QueryInterface(IID_IDXGISurface1, (void**)&surface);
      if (hr != S_OK) {
        printf("Could not query interface\n");
      }

      // Create a factory!
      D2D1_ALPHA_MODE alphaMode = D2D1_ALPHA_MODE_PREMULTIPLIED;
      D2D1_RENDER_TARGET_PROPERTIES props
        = D2D1::RenderTargetProperties(D2D1_RENDER_TARGET_TYPE_DEFAULT,
                                       D2D1::PixelFormat(desc.Format, alphaMode));
      ID2D1Factory* factory;
      D2D1_FACTORY_OPTIONS options;
      hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED,
                             __uuidof(ID2D1Factory),
                             &options,
                             (void**)&factory);
      if (hr != S_OK) {
        printf("Could not create factory\n");
      }

      // Create a render target
      ID2D1RenderTarget* rt;
      hr = factory->CreateDxgiSurfaceRenderTarget(surface, &props, &rt);
      if (hr != S_OK) {
        // Always fails with invalid arg
        printf("Could not create a render target code: %x\n", hr);
      }
                             
    }




}

