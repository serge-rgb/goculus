#ifndef VR_WRAP
#define VR_WRAP

#ifdef __cplusplus
extern "C" {
#endif
    // See LibOVR/Src/OVR_Device.h
    struct RiftInfo {
        // Size of the entire screen, in pixels.
        unsigned  HResolution, VResolution;
        // Physical dimensions of the active screen in meters. Can be used to calculate
        // projection center while considering IPD.
        float     HScreenSize, VScreenSize;
        // Physical offset from the top of the screen to the eye center, in meters.
        // This will usually, but not necessarily be half of VScreenSize.
        float     VScreenCenter;
        // Distance from the eye to screen surface, in meters.
        // Useful for calculating FOV and projection.
        float     EyeToScreenDistance;
        // Distance between physical lens centers useful for calculating distortion center.
        float     LensSeparationDistance;
        // Configured distance between the user's eye centers, in meters. Defaults to 0.064.
        float     InterpupillaryDistance;

        // Radial distortion correction coefficients.
        // The distortion assumes that the input texture coordinates will be scaled
        // by the following equation:
        //   uvResult = uvInput * (K0 + K1 * uvLength^2 + K2 * uvLength^4)
        // Where uvInput is the UV vector from the center of distortion in direction
        // of the mapped pixel, uvLength is the magnitude of that vector, and uvResult
        // the corresponding location after distortion.
        float     DistortionK[4];

        float     ChromaAbCorrection[4];

        // Desktop coordinate position of the screen (can be negative; may not be present on all platforms)
        int       DesktopX, DesktopY;
    };
    void vr_init();

    void vr_get_orientation(float* pitch, float* yaw, float* roll);

    void vr_get_acceleration(float *x, float *y, float *z);

    void vr_finish();

#ifdef __cplusplus
}
#endif

#endif
