//
//  FBYFaceRecognitionViewController.h
//  FBYFaceRecognition_iOS

#import <UIKit/UIKit.h>
#import <Foundation/Foundation.h>

@protocol FaceDetectorDelegate <NSObject>

//upload successful
-(void)sendFaceImage:(UIImage *)faceImage;

//upload fail
-(void)sendFaceImageError;

@end
@interface FBYFaceRecognitionViewController : UIViewController

@property (assign,nonatomic) id<FaceDetectorDelegate> faceDelegate;


@end
