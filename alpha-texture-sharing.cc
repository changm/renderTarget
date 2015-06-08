#include <d3d11.h>
#include <stdio.h>
#include <atlbase.h>


// See bug 1083071. On some drivers, Direct3D 11 CreateShaderResourceView fails
// with E_OUTOFMEMORY.
bool DoesD3D11TextureSharingWorkInternal(ID3D11Device *device, ID3D11Device *otherDevice, DXGI_FORMAT format, UINT bindflags)
{


        CComPtr<ID3D11Texture2D> texture;
        D3D11_TEXTURE2D_DESC desc;
        desc.Width = 32;
        desc.Height = 32;
        desc.MipLevels = 1;
        desc.ArraySize = 1;
        desc.Format = format;
        desc.SampleDesc.Count = 1;
        desc.SampleDesc.Quality = 0;
        desc.Usage = D3D11_USAGE_DEFAULT;
        desc.CPUAccessFlags = 0;
        desc.MiscFlags = D3D11_RESOURCE_MISC_SHARED_KEYEDMUTEX;
        desc.BindFlags = bindflags;
        if (FAILED(device->CreateTexture2D(&desc, NULL, &texture))) {
            return false;
        }

        HANDLE shareHandle;
        CComPtr<IDXGIResource> otherResource;
        if (FAILED(texture->QueryInterface(
            &otherResource)))
        {
            return false;
        }

        if (FAILED(otherResource->GetSharedHandle(&shareHandle))) {
            return false;
        }

        CComPtr<ID3D11Resource> sharedResource;
        CComPtr<ID3D11Texture2D> sharedTexture;
        if (FAILED(otherDevice->OpenSharedResource(shareHandle, __uuidof(ID3D11Resource), (void**)&sharedResource)))
        {
            return false;
        }

        if (FAILED(sharedResource->QueryInterface(&sharedTexture)))
        {
            return false;
        }

        CComPtr<ID3D11ShaderResourceView> sharedView;

        // This if(FAILED()) is the one that actually fails on systems affected by bug 1083071.
        if (FAILED(device->CreateShaderResourceView(sharedTexture, NULL, &sharedView))) {

            return false;
        }


    return true;
}


    int main(int argc, char **argv)
    {
        int size;
        ID3D11Device *device;
        ID3D11Device *device2;
        ID3D11DeviceContext *context;
        D3D_FEATURE_LEVEL level;
        D3D11CreateDevice(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, D3D11_CREATE_DEVICE_DEBUG,
            NULL, 0, D3D11_SDK_VERSION, &device,
            &level, &context);
        D3D11CreateDevice(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, D3D11_CREATE_DEVICE_DEBUG,
            NULL, 0, D3D11_SDK_VERSION, &device2,
            &level, &context);

        if (!DoesD3D11TextureSharingWorkInternal(device, device2, DXGI_FORMAT_A8_UNORM, D3D11_BIND_SHADER_RESOURCE)) {
        }
        printf("Success!\n");

    }