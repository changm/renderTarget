CXX=cl
CXXFLAGS=-Z7
LDLIBS=d3d11.lib dxgi.lib dxguid.lib d2d1.lib
all: texture-perf alpha-texture-sharing max-texture shared-surface release-sync-rendertarget firefox.exe

max-texture:


# copy to firefox.exe so that optimus uses the right driver
firefox.exe: release-sync-recreate-rendertargetview
	cp release-sync-recreate-rendertargetview.exe firefox.exe

shared-surface:
