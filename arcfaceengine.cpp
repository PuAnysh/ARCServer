#include "arcfaceengine.h"

ARCFaceEngine::ARCFaceEngine()
{
    FREngine = nullptr;
    FDEngine = nullptr;
    pFDWorkBUF = nullptr;
    pFRWorkBUF = nullptr;
    //pdb = nullptr;
}

int ARCFaceEngine::InitEngine()
{
    /*
     * check  engines before create
    */
    if(FREngine != nullptr || FDEngine != nullptr ||
            pFDWorkBUF != nullptr || pFRWorkBUF != nullptr){
        fprintf(stderr , "the engine may be running");
        return -1;
    }
    /*
     * initilize the FD engine
    */
    pFDWorkBUF = (MByte*)malloc(WORKSIZE);
    if(pFDWorkBUF == nullptr) {
        fprintf(stderr , "fail to malloc FDWorkbuf\r\n");
        return -1;
    }
    int ret_FD = AFD_FSDK_InitialFaceEngine(APPID , FD_SDKKEY , pFDWorkBUF , WORKSIZE , &FDEngine , AFD_FSDK_OPF_0_ONLY , 16 , 1);
    if(ret_FD != MOK) {
        fprintf(stderr , "fail to AFD_FSDK_InitialFaceEngine(): 0x%x\r\n" , ret_FD);
        free(pFDWorkBUF);
        return -1;
    }
    /*
     * initilize the FR engine
    */
    pFRWorkBUF  = (MByte*)malloc(WORKSIZE);
    if(pFRWorkBUF == nullptr){
        fprintf(stderr , "fail to malloc FRWorkbuf");
        return -1;
    }
    int ret_FR = AFR_FSDK_InitialEngine(APPID , FR_SDKKEY , pFRWorkBUF , WORKSIZE , &FREngine);
    if(ret_FR != MOK){
        fprintf(stderr , "fail to AFD_FSDK_InitialEngine(): 0x%x\r\n" , ret_FR);
        free(pFRWorkBUF);
        return -1;
    }
    return 0;
}

int ARCFaceEngine::UninitialEngine()
{
    /*
     * unintall the FD engine
    */
    int FDret = AFD_FSDK_UninitialFaceEngine(FDEngine);
    if(FDret == MOK){
        free(pFDWorkBUF);
        pFDWorkBUF = nullptr;
        FDEngine = nullptr;
    }
    else{
        fprintf(stderr , "fail to AFD_FSDK_UninitialFaceEngine(): 0x%x\r\n" , FDret);
    }

    /*
     * unintall the FR engine
    */
    int FRret = AFR_FSDK_UninitialEngine(FREngine);
    if(FRret == MOK){
        free(pFRWorkBUF);
        pFRWorkBUF = nullptr;
        FREngine = nullptr;
    }
    else{
        fprintf(stderr , "fail to AFD_FSDK_UninitialEngine() 0x%x\r\n" , FRret);
    }
    /*
     * return flag
    */
    if(FRret !=MOK || FDret != MOK){
        return -1;
    }
    else{
        return 0;
    }
}

int ARCFaceEngine::getFaceInputImg(unsigned char *data, int width, int height, MUInt32 format , ASVLOFFSCREEN &inputImg)
{
    /*
     * this block of code aims to get the rectangle of face
    */
    inputImg.u32PixelArrayFormat = format;
    inputImg.i32Width = width;
    inputImg.i32Height = height;
    inputImg.ppu8Plane[0] = data;
    if (ASVL_PAF_I420 == inputImg.u32PixelArrayFormat) {
        inputImg.pi32Pitch[0] = inputImg.i32Width;
        inputImg.pi32Pitch[1] = inputImg.i32Width/2;
        inputImg.pi32Pitch[2] = inputImg.i32Width/2;
        inputImg.ppu8Plane[1] = inputImg.ppu8Plane[0] + inputImg.pi32Pitch[0] * inputImg.i32Height;
        inputImg.ppu8Plane[2] = inputImg.ppu8Plane[1] + inputImg.pi32Pitch[1] * inputImg.i32Height/2;
    } else if (ASVL_PAF_NV12 == inputImg.u32PixelArrayFormat) {
        inputImg.pi32Pitch[0] = inputImg.i32Width;
        inputImg.pi32Pitch[1] = inputImg.i32Width;
        inputImg.ppu8Plane[1] = inputImg.ppu8Plane[0] + (inputImg.pi32Pitch[0] * inputImg.i32Height);
    } else if (ASVL_PAF_NV21 == inputImg.u32PixelArrayFormat) {
        inputImg.pi32Pitch[0] = inputImg.i32Width;
        inputImg.pi32Pitch[1] = inputImg.i32Width;
        inputImg.ppu8Plane[1] = inputImg.ppu8Plane[0] + (inputImg.pi32Pitch[0] * inputImg.i32Height);
    } else if (ASVL_PAF_YUYV == inputImg.u32PixelArrayFormat) {
        inputImg.pi32Pitch[0] = inputImg.i32Width*2;
    } else if (ASVL_PAF_I422H == inputImg.u32PixelArrayFormat) {
        inputImg.pi32Pitch[0] = inputImg.i32Width;
        inputImg.pi32Pitch[1] = inputImg.i32Width / 2;
        inputImg.pi32Pitch[2] = inputImg.i32Width / 2;
        inputImg.ppu8Plane[1] = inputImg.ppu8Plane[0] + inputImg.pi32Pitch[0] * inputImg.i32Height;
        inputImg.ppu8Plane[2] = inputImg.ppu8Plane[1] + inputImg.pi32Pitch[1] * inputImg.i32Height;
    } else if (ASVL_PAF_LPI422H == inputImg.u32PixelArrayFormat) {
        inputImg.pi32Pitch[0] = inputImg.i32Width;
        inputImg.pi32Pitch[1] = inputImg.i32Width;
        inputImg.ppu8Plane[1] = inputImg.ppu8Plane[0] + (inputImg.pi32Pitch[0] * inputImg.i32Height);
    } else if (ASVL_PAF_RGB24_B8G8R8 == inputImg.u32PixelArrayFormat) {
        inputImg.pi32Pitch[0] = inputImg.i32Width*3;
    } else {
        fprintf(stderr, "unsupported Image format: 0x%x\r\n",inputImg.u32PixelArrayFormat);
        return -1;
    }
    return 0;
}

int ARCFaceEngine::getFeature(unsigned char *data, int width, int height, MUInt32 format, LPAFR_FSDK_FACEMODEL localmodel)
{
    ASVLOFFSCREEN inputImg;
    LPAFD_FSDK_FACERES faceResult;

    getFaceInputImg(data , width , height , format , inputImg);
    int FDret = AFD_FSDK_StillImageFaceDetection(FDEngine  , &inputImg , &faceResult);
    if(FDret != MOK){
        fprintf(stderr , "fail to AFD_FSDK_StillImageFaceDetection(): 0x%x\r\n" , FDret);
        return  -1;
    }
    if(faceResult->nFace != 1){
        fprintf(stderr , "no face in picture \r\n");
        return -1;
    }
    AFR_FSDK_FACEINPUT fr_inputface;
    fr_inputface.lOrient = AFR_FSDK_FOC_0;
    fr_inputface.rcFace = faceResult->rcFace[0];
    /********debug*********/
    fprintf(stderr , "face number %d\r\n" , faceResult->nFace);
    for (int i = 0; i < faceResult->nFace; i++) {
        fprintf(stderr , "face %d:(%d,%d,%d,%d)\r\n", i,
               faceResult->rcFace[i].left, faceResult->rcFace[i].top,
               faceResult->rcFace[i].right, faceResult->rcFace[i].bottom);

    }
    /********debug*********/


    int FRret = AFR_FSDK_ExtractFRFeature(FREngine , &inputImg , &fr_inputface , localmodel);
    if(FRret != MOK){
        fprintf(stderr , "fail to AFR_FSDK_ExtractFRFeature(): 0x%x\r\n" , FRret);
        return -1;
    }
    return FRret;
}

int ARCFaceEngine::compareFeature(AFR_FSDK_FACEMODEL* model ,AFR_FSDK_FACEMODEL* localmodel)
{
    MFloat res;
    AFR_FSDK_FacePairMatching(FREngine , model, localmodel , &res);
    printf("res is   %.8f\r\n" , res);
    if(res > 0.95){
        return 1;
    }
    return 0;
}
