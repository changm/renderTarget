#include <d3d11.h>
#include <stdio.h>
#define WIDTH 200

int buf[WIDTH*200];

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
    desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
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
      ID3D11Texture2D *texture2;
      LARGE_INTEGER before;
      LARGE_INTEGER after;
      QueryPerformanceCounter(&before);
      result = device->CreateTexture2D(&desc, NULL, &texture2);
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
      D3D11_MAPPED_SUBRESOURCE map;
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
      context->CopyResource(texture, texture2);
      QueryPerformanceCounter(&after);
      time = after.QuadPart - before.QuadPart;
      printf("%x copy time: %.1fus\n", result, time*ticksToMS);

    }




}

