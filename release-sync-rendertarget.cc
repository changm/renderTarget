#include <d3d11.h>
#include <stdio.h>
#include <assert.h>
int main(int argc, char **argv)
{
  int size;
  ID3D11Device *device;
  ID3D11DeviceContext *context;
  D3D_FEATURE_LEVEL level;
  D3D11CreateDevice(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, D3D11_CREATE_DEVICE_DEBUG,
                    NULL, 0, D3D11_SDK_VERSION, &device,
                    &level, &context);

  HRESULT hr;
  ID3D11Texture2D *texture;
  ID3D11Texture2D *texture2;
  {
    D3D11_TEXTURE2D_DESC desc;
    desc.Width = 400;
    desc.Height = 400;
    desc.MipLevels = 1;
    desc.ArraySize = 1;
    desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    desc.SampleDesc.Count = 1;
    desc.SampleDesc.Quality = 0;
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.CPUAccessFlags = 0;
    desc.MiscFlags = D3D11_RESOURCE_MISC_SHARED_KEYEDMUTEX;
    desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
    hr = device->CreateTexture2D(&desc, NULL, &texture);
    desc.MiscFlags = 0;
    hr = device->CreateTexture2D(&desc, NULL, &texture2);
  }
  printf("texture %x %p\n", hr, texture);
  printf("texture2 %x %p\n", hr, texture2);

  IDXGIResource* otherResource(NULL);
  hr = texture->QueryInterface( __uuidof(IDXGIResource), (void**)&otherResource );
  IDXGIKeyedMutex* keyedMutex(NULL);
  hr = texture->QueryInterface( __uuidof(IDXGIKeyedMutex), (void**)&keyedMutex);
  printf("%x %p\n", hr, keyedMutex);
  keyedMutex->AcquireSync(0, INFINITE);

  D3D11_RENDER_TARGET_VIEW_DESC rtDesc;
  rtDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
  rtDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
  rtDesc.Texture2D.MipSlice = 0;
  ID3D11RenderTargetView *rtView;
  ID3D11RenderTargetView *rtView2;
  hr = device->CreateRenderTargetView(texture, &rtDesc, &rtView);
  hr = device->CreateRenderTargetView(texture2, &rtDesc, &rtView2);

  ID3D11RenderTargetView *views[2];
  views[0] = rtView;
  views[1] = rtView2;
  context->OMSetRenderTargets(2, views, NULL);


  context->OMGetRenderTargets(2, views, NULL);
  printf("views[0] %x\n", views[0]);
  printf("views[1] %x\n", views[1]);
  assert(views[0] != NULL);
  views[0]->Release();

  keyedMutex->ReleaseSync(0);

  // calling ReleaseSync will unbind the surface
  // from being a RenderTarget
  context->OMGetRenderTargets(2, views, NULL);
  printf("views[0] %x\n", views[0]);
  printf("views[1] %x\n", views[1]);
  assert(views[0] == NULL);
  assert(views[1] != NULL);

  otherResource->Release();
  texture->Release();
  rtView->Release();
  context->Release();
  device->Release();

}

