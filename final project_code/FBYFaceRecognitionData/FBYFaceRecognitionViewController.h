//
//  FBYFaceRecognitionViewController.h
//  FBYFaceRecognition_iOS

#import <UIKit/UIKit.h>
#import <Foundation/Foundation.h>

@protocol FaceDetectorDelegate <NSObject>

//图片上传成功
-(void)sendFaceImage:(UIImage *)faceImage;

//图片上传失败
-(void)sendFaceImageError;

@end
@interface FBYFaceRecognitionViewController : UIViewController

@property (assign,nonatomic) id<FaceDetectorDelegate> faceDelegate;


@end
