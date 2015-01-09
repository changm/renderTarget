#include <d3d11.h>
#include <stdio.h>
#include <assert.h>


static void doCrash(ID3D11Device *device, ID3D11DeviceContext *deviceContext)
{
    int backbufferWidth = 32; int backbufferHeight = 32;
    ID3D11Texture2D* mOffscreenTexture;
    IDXGIKeyedMutex* mKeyedMutex;

    D3D11_TEXTURE2D_DESC offscreenTextureDesc = { 0 };
    offscreenTextureDesc.Width = backbufferWidth;
    offscreenTextureDesc.Height = backbufferHeight;
    offscreenTextureDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    offscreenTextureDesc.MipLevels = 0;
    offscreenTextureDesc.ArraySize = 1;
    offscreenTextureDesc.SampleDesc.Count = 1;
    offscreenTextureDesc.SampleDesc.Quality = 0;
    offscreenTextureDesc.Usage = D3D11_USAGE_DEFAULT;
    offscreenTextureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
    offscreenTextureDesc.CPUAccessFlags = 0;
    offscreenTextureDesc.MiscFlags = D3D11_RESOURCE_MISC_SHARED_KEYEDMUTEX;

    HRESULT result = device->CreateTexture2D(&offscreenTextureDesc, NULL, &mOffscreenTexture);

    result = mOffscreenTexture->QueryInterface(__uuidof(IDXGIKeyedMutex), (void**)&mKeyedMutex);

    D3D11_RENDER_TARGET_VIEW_DESC offscreenRTVDesc;
    offscreenRTVDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    offscreenRTVDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
    offscreenRTVDesc.Texture2D.MipSlice = 0;

    ID3D11RenderTargetView *mOffscreenRTView;
    result = device->CreateRenderTargetView(mOffscreenTexture, &offscreenRTVDesc, &mOffscreenRTView);

    // Acquire and clear
    mKeyedMutex->AcquireSync(0, INFINITE);
    FLOAT color1[4] = { 1, 1, 0.5, 1 };
    deviceContext->ClearRenderTargetView(mOffscreenRTView, color1);
    mKeyedMutex->ReleaseSync(0);


    mKeyedMutex->AcquireSync(0, INFINITE);
    FLOAT Color1[4] = { 1, 1, 0, 1 };

    deviceContext->ClearRenderTargetView(mOffscreenRTView, Color1);
    D3D11_TEXTURE2D_DESC desc;

    mOffscreenTexture->GetDesc(&desc);
    desc.Usage = D3D11_USAGE_STAGING;
    desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
    desc.MiscFlags = 0;
    desc.BindFlags = 0;
    ID3D11Texture2D* cpuTexture;
    device->CreateTexture2D(&desc, NULL, &cpuTexture);

    deviceContext->CopyResource(cpuTexture, mOffscreenTexture);

    D3D11_MAPPED_SUBRESOURCE mapped;
    deviceContext->Map(cpuTexture, 0, D3D11_MAP_READ, 0, &mapped);
    int resultColor = *(int*)mapped.pData;
    deviceContext->Unmap(cpuTexture, 0);
    cpuTexture->Release();

    printf("%x\n", resultColor);
    if (resultColor != 0xffffff00) {
        *(char*)0 = 6;
    }

    mKeyedMutex->ReleaseSync(0);
}

int main(int argc, char **argv)
{
    int size;
    ID3D11Device *device;
    ID3D11DeviceContext *context;

    D3D_FEATURE_LEVEL level;
    D3D11CreateDevice(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, D3D11_CREATE_DEVICE_DEBUG,
        NULL, 0, D3D11_SDK_VERSION, &device,
        &level, &context);

    doCrash(device, context);
}

