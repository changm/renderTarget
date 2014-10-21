#include <d3d11.h>
#include <stdio.h>
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
  do {
    trial_size = size;
    result = 0x800000;
    D3D11_TEXTURE2D_DESC desc;
    desc.Width = trial_size;
    desc.Height = 1;
    desc.MipLevels = 1;
    desc.ArraySize = 1;
    desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    desc.SampleDesc.Count = 4;
    desc.SampleDesc.Quality = 0;
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.CPUAccessFlags = 0;
    desc.MiscFlags = 0;
    desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    ID3D11Texture2D *texture;
    __try {
      result = device->CreateTexture2D(&desc, NULL, &texture);
    } __except (EXCEPTION_EXECUTE_HANDLER) {
      printf("CRASHED\n");
    }
    if (result == 0)
      size_low = trial_size;
    else
      size = trial_size;
    trial_size = size_low + (size - size_low) / 2;
    printf("%d (%d %d) %x %p\n", size_low, size, trial_size, result, texture);
  } while (0 && size > 0 && (size - size_low) > 1);
}

