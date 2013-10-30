#include <OVR.h>
#include "vr_wrap.h"

#ifdef VR_ENABLE_TEST
#include <unistd.h>
#endif

using namespace OVR;

struct RiftState;
RiftState *gState;

struct RiftState {
    RiftState() {
        System::Init(Log::ConfigureDefaultLog(LogMask_All));
        manager = *DeviceManager::Create();
        device = *manager->EnumerateDevices<HMDDevice>().CreateDevice();
        if (!device) {
            fprintf(stderr, "Error, could not create device.\n");
            exit(-1);
        }
        HMDInfo info;
        if (device->GetDeviceInfo(&info)) {
        }
        sensor = *device->GetSensor();
        if (!sensor) {
            fprintf(stderr,"Error: No sensor found.\n");
            return;
        }
        s_fusion.AttachToSensor(sensor);
        if (!s_fusion.IsAttachedToSensor()) {
            fprintf(stderr, "Error: Not attached to sensor.\n");
        }

    }

    ~RiftState() {
        printf("Destroying rift state\n");
    }

    Ptr<DeviceManager> manager;
    Ptr<HMDDevice> device;
    Ptr<SensorDevice> sensor;
    SensorFusion s_fusion;
};

/* extern "C" { */
    void vr_init() {
        gState = new RiftState;
    }

    RiftInfo vr_get_info() {
        RiftInfo res;
        HMDInfo info;
        if (!gState->device->GetDeviceInfo(&info)) {
            fprintf(stderr, "Error: Couldn't get HMD info.\n");
            return res;
        }
#define HMD_COPY(name) res.name = info.name
        HMD_COPY(HResolution);
        HMD_COPY(VResolution);
        HMD_COPY(HScreenSize);
        HMD_COPY(VScreenSize);
        HMD_COPY(VScreenCenter);
        HMD_COPY(EyeToScreenDistance);
        HMD_COPY(LensSeparationDistance);
        HMD_COPY(InterpupillaryDistance);
        memcpy(res.DistortionK,        info.DistortionK,        4 * sizeof(float));
        memcpy(res.ChromaAbCorrection, info.ChromaAbCorrection, 4 * sizeof(float));
        HMD_COPY(DesktopX);
        HMD_COPY(DesktopY);
#undef HMD_COPY

        return res;
    }

    void vr_get_orientation(float* pitch, float* yaw, float* roll) {
        Quatf orient = gState->s_fusion.GetPredictedOrientation();
        orient.GetEulerAngles<Axis_Y, Axis_X, Axis_Z>(yaw, pitch, roll);
    }

    void vr_get_acceleration(float *x, float *y, float *z) {
        if (!gState->s_fusion.IsAttachedToSensor()) {
            fprintf(stderr, "Error: Not attached to sensor.\n");
        }
        Vector3f acc = gState->s_fusion.GetAcceleration();
        *x = acc.x;
        *y = acc.y;
        *z = acc.z;
    }

    void vr_finish() {
        delete gState;
        System::Destroy();
    }
/* } */

#ifdef VR_ENABLE_TEST
int main(int argc, char *argv[]) {
    vr_init();

    RiftInfo info = vr_get_info();

    printf("Resolution %dx%d\n", info.HResolution,             info.VResolution);
    printf("Size       %fx%f\n", info.HScreenSize,             info.VScreenSize);
    printf("Center     %f\n",    info.VScreenCenter);
    printf("IPD:       %f\n",    info.InterpupillaryDistance);

    for (int i = 0; i < 10; ++i) {
        float x,y,z,w;
        vr_get_orientation(&x, &y, &z);
        printf("ORIENT: %f %f %f\n", x, y ,z);

        vr_get_acceleration(&x, &y, &z);
        printf("ACC: %f %f %f\n", x, y ,z);

        usleep(10000);
    }

    vr_finish();

    return 0;
}
#endif
