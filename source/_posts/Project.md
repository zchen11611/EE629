
---
title:  'Final Project- Face recognition_live detection'
categories: 
tags: 
toc: true
comments: true
date: 2020-12-13 21:19:00
updated: 2020-12-13 21:19:06
keywords: 
description: 
---

## Final Project- Face recognition_live detection

### 1.Introduction

The well-known Alipay (Chinese payment platform) uses face++ services to realize face recognition. In actual projects, iFLYTEK’s face recognition SDK is used for secondary packaging to achieve live recognition.
In actual use, many apps have implemented fingerprint login, gesture login, third-party login, and face-swiping login in order to ensure the safety of users. In addition to the regular account and password login, I will Share how the project realizes the live detection of face recognition, which is the most basic implementation for realizing face recognition login.

### 2.Project realization ideas

1. Click the recognition button to call the camera
2. CameraRules class, detect camera permissions
3. Initialize page, create camera page, create mouth data and head shaking data
4. Turn on recognition, face frame recognition
5. Face recognition, face recognition determines whether a face is detected
6. After detecting the face, determine the location
7. The position is judged right, judge whether to open the mouth
8. Open your mouth to judge, verify whether you shake your head
9. Shake your head to judge the completion, 3 seconds countdown to take pictures
10. After taking the picture, choose to retake or upload the picture
11. Select retake and repeat steps 5-9, select upload to recall the image data
12. Data clean

### 3.Source code

**Click the recognition button to call the camera**

```java
if([CameraRules isCapturePermissionGranted]){
        [self setDeviceAuthorized:YES];
    }
    else{
        dispatch_async(dispatch_get_main_queue(), ^{
            NSString* info=@"No camera permission"
            [self showAlert:info];
            [self setDeviceAuthorized:NO];
        });
    }

```

**CameraRules class, detect camera permissions**

```java
//Detect camera permissions
+(BOOL)isCapturePermissionGranted{
    if([AVCaptureDevice respondsToSelector:@selector(authorizationStatusForMediaType:)]){
        AVAuthorizationStatus authStatus = [AVCaptureDevice authorizationStatusForMediaType:AVMediaTypeVideo];
        if(authStatus ==AVAuthorizationStatusRestricted || authStatus ==AVAuthorizationStatusDenied){
            return NO;
        }
        else if(authStatus==AVAuthorizationStatusNotDetermined){
            dispatch_semaphore_t sema = dispatch_semaphore_create(0);
            __block BOOL isGranted=YES;
            [AVCaptureDevice requestAccessForMediaType:AVMediaTypeVideo completionHandler:^(BOOL granted) {
                isGranted=granted;
                dispatch_semaphore_signal(sema);
            }];
            dispatch_semaphore_wait(sema, DISPATCH_TIME_FOREVER);
            return isGranted;
        }
        else{
            return YES;
        }
    }
    else{
        return YES;
    }
}
```

**Initialize page, create camera page, create mouth data and head shaking data**

```java
 //Create camera page, create mouth opening data and head shaking data
    [self faceUI];
    [self faceCamera];
    [self faceNumber];
```

**Turn on recognition, face frame recognition**

```java
    float cx = (left+right)/2;
    float cy = (top + bottom)/2;
    float w = right - left;
    float h = bottom - top;
    float ncx = cy ;
    float ncy = cx ;
    
    CGRect rectFace = CGRectMake(ncx-w/2 ,ncy-w/2 , w, h);
    
    if(!isFrontCamera){
        rectFace=rSwap(rectFace);
        rectFace=rRotate90(rectFace, faceImg.height, faceImg.width);
    }
    
    BOOL isNotLocation = [self identifyYourFaceLeft:left right:right top:top bottom:bottom];
    
    if (isNotLocation==YES) {
        return nil;
    }

```

**Face recognition, face recognition determines whether a face is detected**

```java
    for(id key in keys){
        id attr=[landmarkDic objectForKey:key];
        if(attr && [attr isKindOfClass:[NSDictionary class]]){
            
            if(!isFrontCamera){
                p=pSwap(p);
                p=pRotate90(p, faceImg.height, faceImg.width);
            }
            if (isCrossBorder == YES) {
                [self delateNumber];
                return nil;
            }
            p=pScale(p, widthScaleBy, heightScaleBy);
            
            [arrStrPoints addObject:NSStringFromCGPoint(p)];
            
        }
    }

```

**After detecting the face, determine the location**

```java
    if (right - left < 230 || bottom - top < 250) {
        self.textLabel.text = @"too far";
        [self delateNumber];
        isCrossBorder = YES;
        return YES;
    }else if (right - left > 320 || bottom - top > 320) {
        self.textLabel.text = @"too close";
        [self delateNumber];
        isCrossBorder = YES;
        return YES;
    }else{
        if (isJudgeMouth != YES) {
            self.textLabel.text = @"Please repeat opening mouth";
            [self tomAnimationWithName:@"openMouth" count:2];
            
            if (left < 100 || top < 100 || right > 460 || bottom > 400) {
                isCrossBorder = YES;
                isJudgeMouth = NO;
                self.textLabel.text = @"Please adjust the position first";
                [self delateNumber];
                return YES;
            }
        }else if (isJudgeMouth == YES && isShakeHead != YES) {
            self.textLabel.text = @"Please repeat shaking your head";
            [self tomAnimationWithName:@"shakeHead" count:4];
            number = 0;
        }else{
            takePhotoNumber += 1;
            if (takePhotoNumber == 2) {
                [self timeBegin];
            }
        }
        isCrossBorder = NO;
    }

```

**The position is judged right, judge whether to open the mouth**

```java
    if (rightX && leftX && upperY && lowerY && isJudgeMouth != YES) {
        
        number ++;
        if (number == 1 || number == 300 || number == 600 || number ==900) {
            mouthWidthF = rightX - leftX < 0 ? abs(rightX - leftX) : rightX - leftX;
            mouthHeightF = lowerY - upperY < 0 ? abs(lowerY - upperY) : lowerY - upperY;
            NSLog(@"%d,%d",mouthWidthF,mouthHeightF);
        }else if (number > 1200) {
            [self delateNumber];
            [self tomAnimationWithName:@"openMouth" count:2];
        }
        
        mouthWidth = rightX - leftX < 0 ? abs(rightX - leftX) : rightX - leftX;
        mouthHeight = lowerY - upperY < 0 ? abs(lowerY - upperY) : lowerY - upperY;
        NSLog(@"%d,%d",mouthWidth,mouthHeight);
        NSLog(@"Before opening your mouth：width=%d，height=%d",mouthWidthF - mouthWidth,mouthHeight - mouthHeightF);
        if (mouthWidth && mouthWidthF) {
           
            if (mouthHeight - mouthHeightF >= 20 && mouthWidthF - mouthWidth >= 15) {
                isJudgeMouth = YES;
                imgView.animationImages = nil;
            }
        }
    }

```

**Open your mouth to judge, verify whether you shake your head**

```java
if ([key isEqualToString:@"mouth_middle"] && isJudgeMouth == YES) {
        
        if (bigNumber == 0 ) {
            firstNumber = p.x;
            bigNumber = p.x;
            smallNumber = p.x;
        }else if (p.x > bigNumber) {
            bigNumber = p.x;
        }else if (p.x < smallNumber) {
            smallNumber = p.x;
        }
       
        if (bigNumber - smallNumber > 60) {
            isShakeHead = YES;
            [self delateNumber];
        }
    }

```

**Shake your head to judge the completion, 3 seconds countdown to take pictures**

```java
if(timeCount >= 1)
    {
        self.textLabel.text = [NSString  stringWithFormat:@"%ld s后拍照",(long)timeCount];
    }
    else
    {
        [theTimer invalidate];
        theTimer=nil;
        
        [self didClickTakePhoto];
    }

```

**After taking the picture, choose to retake or upload the picture**

```java
-(void)didClickPhotoAgain
{
    [self delateNumber];
    
    [self.previewLayer.session startRunning];
    self.textLabel.text = @"Pls adjust your position";
    
    [backView removeFromSuperview];
    
    isJudgeMouth = NO;
    isShakeHead = NO;
    
}

```

**Select retake and repeat steps 5-9, select upload to recall the image data**

```java
-(void)didClickUpPhoto
{
    //successfully upload theimage
    [self.faceDelegate sendFaceImage:imageView.image];
    [self.navigationController popViewControllerAnimated:YES];
}

```

**Data clean**

```java
-(void)delateNumber
{
    number = 0;
    takePhotoNumber = 0;
    
    mouthWidthF = 0;
    mouthHeightF = 0;
    mouthWidth = 0;
    mouthHeight = 0;
    
    smallNumber = 0;
    bigNumber = 0;
    firstNumber = 0;
    
    imgView.animationImages = nil;
    imgView.image = [UIImage imageNamed:@"shakeHead0"];
}

```



### 4.SDK download

Because the Xunfei face recognition SDK is used in the project, you need to go to the Xunfei open platform to create an application and download the SDK



### 5.add SDK to the demo

- Download the demo and import the FBYFaceData folder in the demo into the project.
- Introduce FBYFaceRecognitionViewController in the project

```java
#import "FBYFaceRecognitionViewController.h"
```

- Add code to the click event of the item recognition button

```java
-(void)pushToFaceStreamDetectorVC
{
    FBYFaceRecognitionViewController *faceVC = [[FBYFaceRecognitionViewController alloc]init];
    faceVC.faceDelegate = self;
    [self.navigationController pushViewController:faceVC animated:YES];
}
```

