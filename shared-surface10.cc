#include <d3d10_1.h>
#include <stdio.h>
int main(int argc, char **argv)
{
  ID3D10Device1 *device;
  ID3D10Device1 *&context = device;
  IDXGIFactory1 * pFactory = NULL;
  IDXGIAdapter * pAdapter = NULL;
  CreateDXGIFactory1(__uuidof(IDXGIFactory1), (void**)&pFactory);
  pFactory->EnumAdapters(0, &pAdapter);
  D3D10CreateDevice1(pAdapter, D3D10_DRIVER_TYPE_HARDWARE, NULL, D3D10_CREATE_DEVICE_DEBUG | D3D10_CREATE_DEVICE_BGRA_SUPPORT,
    D3D10_FEATURE_LEVEL_10_1, D3D10_1_SDK_VERSION, &device);

  HRESULT hr;
  ID3D10Texture2D *texture;
  ID3D10Texture2D *textureCPU;
  {
    D3D10_TEXTURE2D_DESC desc;
    desc.Width = 400;
    desc.Height = 400;
    desc.MipLevels = 1;
    desc.ArraySize = 1;
    desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    desc.SampleDesc.Count = 1;
    desc.SampleDesc.Quality = 0;
    desc.Usage = D3D10_USAGE_DEFAULT;
    desc.CPUAccessFlags = 0;
    desc.MiscFlags = D3D10_RESOURCE_MISC_SHARED_KEYEDMUTEX;
    desc.BindFlags = D3D10_BIND_RENDER_TARGET | D3D10_BIND_SHADER_RESOURCE;
    hr = device->CreateTexture2D(&desc, NULL, &texture);
  }
  printf("texture %x %p\n", hr, texture);
  {
    D3D10_TEXTURE2D_DESC desc;
    desc.Width = 400;
    desc.Height = 400;
    desc.MipLevels = 1;
    desc.ArraySize = 1;
    desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    desc.SampleDesc.Count = 1;
    desc.SampleDesc.Quality = 0;
    desc.Usage = D3D10_USAGE_STAGING;
    desc.CPUAccessFlags = D3D10_CPU_ACCESS_READ;
    desc.MiscFlags = 0;
    desc.BindFlags = 0;
    hr = device->CreateTexture2D(&desc, NULL, &textureCPU);
  }

  printf("textureCPU %x %p\n", hr, textureCPU);
  HANDLE shareHandle;
  IDXGIResource* otherResource(NULL);
  hr = texture->QueryInterface( __uuidof(IDXGIResource), (void**)&otherResource );

  IDXGIKeyedMutex* keyedMutex(NULL);
  hr = texture->QueryInterface( __uuidof(IDXGIKeyedMutex), (void**)&keyedMutex);
  printf("%x %p\n", hr, keyedMutex);
  keyedMutex->AcquireSync(0, INFINITE);

  hr = otherResource->GetSharedHandle(&shareHandle);
  printf("%x %p\n", hr, shareHandle);

  D3D10_RENDER_TARGET_VIEW_DESC rtDesc;
  rtDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
  rtDesc.ViewDimension = D3D10_RTV_DIMENSION_TEXTURE2D;
  rtDesc.Texture2D.MipSlice = 0;
  ID3D10RenderTargetView *rtView;
  hr = device->CreateRenderTargetView(texture, &rtDesc, &rtView);
  printf("%x %p\n", hr, rtView);
  FLOAT clearColor[4] = {0.5, 0, 0.5, 0.5};
  context->ClearRenderTargetView(rtView, clearColor);
  context->Flush();
  keyedMutex->ReleaseSync(0);

  ID3D10Resource *sharedResource;
  ID3D10Texture2D *sharedTexture;
  IDXGIKeyedMutex *sharedMutex;
  hr = device->OpenSharedResource(shareHandle, __uuidof(ID3D10Resource), (void**)(&sharedResource));

  sharedResource->QueryInterface(__uuidof(ID3D10Texture2D), (void**)(&sharedTexture));
  sharedResource->QueryInterface(__uuidof(IDXGIKeyedMutex), (void**)(&sharedMutex));
  printf("OpenShared %x %p %p\n", hr, sharedResource, sharedTexture);
  /*context->CopyResource(textureCPU, texture);
  {
    D3D10_MAPPED_TEXTURE2D mapped;
    hr = textureCPU->Map(0, D3D10_MAP_READ, 0, &mapped);
    printf("%x %p\n", hr, mapped.pData);
    printf("color %x\n", *(int*)mapped.pData);
  }*/
  sharedMutex->AcquireSync(0, INFINITE);

  ID3D10ShaderResourceView *sharedView;
  D3D10_SHADER_RESOURCE_VIEW_DESC viewDesc;
  viewDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
  viewDesc.ViewDimension = D3D10_SRV_DIMENSION_TEXTURE2D;
  viewDesc.Texture2D.MipLevels = 1;
  viewDesc.Texture2D.MostDetailedMip = 0;
  hr = device->CreateShaderResourceView(sharedTexture, &viewDesc, &sharedView);
  if (hr == E_OUTOFMEMORY) {
    printf("XXXXXXX OUTOFMEMORY XXXXXXXXXXXXXXX\n");
  }
  printf("CreateShaderResource hr: %x\n", hr);
  context->CopyResource(textureCPU, sharedTexture);
  {
    D3D10_MAPPED_TEXTURE2D mapped;
    hr = textureCPU->Map(0, D3D10_MAP_READ, 0, &mapped);
    printf("%x %p\n", hr, mapped.pData);
    printf("color %x\n", *(int*)mapped.pData);
  }
  sharedMutex->ReleaseSync(0);


  otherResource->Release();
  texture->Release();
  textureCPU->Release();
  rtView->Release();
  context->Release();
  device->Release();

}

