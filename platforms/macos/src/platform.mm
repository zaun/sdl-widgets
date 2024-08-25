#include <pwd.h>
#include <functional>
#include <SDL3/SDL.h>
#include <stdexcept>
#include <unistd.h>
#include "debug.h"
#include "platform.h"

#import <AVFoundation/AVFoundation.h>
#import <Foundation/Foundation.h>

@interface VideoPlayer : NSObject

@property (nonatomic, strong) AVPlayer *player;
@property (nonatomic, strong) AVPlayerItemVideoOutput *videoOutput;
@property (nonatomic, assign) CMTime duration;
@property (nonatomic, assign) int width;
@property (nonatomic, assign) int height;
@property (nonatomic, assign) SDL_Texture *texture;
@property (nonatomic, assign) BOOL repeatMode;
@property (nonatomic, copy) void (^locationCallback)(float currentTime, float maxTime);
@property (nonatomic, strong) dispatch_source_t timer;
@property (nonatomic, assign) BOOL isSeeking;
@property (nonatomic, assign) BOOL isPausedForSeek;
@property (nonatomic, strong) dispatch_source_t seekTimer;

- (instancetype)initWithFilePath:(const std::string&)filePath;
- (void)observeValueForKeyPath:(NSString *)keyPath
                      ofObject:(id)object
                        change:(NSDictionary<NSKeyValueChangeKey,id> *)change
                       context:(void *)context;
- (void)play;
- (void)pause;
- (void)stop;
- (void)seekToTime:(CMTime)time;
- (CMTime)getDuration;
- (CMTime)getCurrentTime;
- (void)setLocationCallback:(void (^)(float currentTime, float maxTime))callback;
- (void)setRepeatMode:(BOOL)repeatMode;
- (void)render:(SDL_Renderer*)renderer inRect:(SDL_Rect&)contentArea;

@end

@implementation VideoPlayer

- (instancetype)initWithFilePath:(const std::string&)filePath {
  self = [super init];
  if (self) {
    self.texture = nullptr;
    self.repeatMode = NO;

    NSURL *videoURL = [NSURL fileURLWithPath:[NSString stringWithUTF8String:filePath.c_str()]];
    if (![[NSFileManager defaultManager] fileExistsAtPath:videoURL.path]) {
      LOG(PLATFORM, "File does not exist at path: %s", filePath.c_str());
      return nil;
    }

    self.player = [[AVPlayer alloc] initWithURL:videoURL];
    NSDictionary *pixelBufferAttributes = @{
      (NSString *)kCVPixelBufferPixelFormatTypeKey: @(kCVPixelFormatType_32BGRA)
    };

    self.videoOutput = [[AVPlayerItemVideoOutput alloc] initWithPixelBufferAttributes:pixelBufferAttributes];
    [self.player.currentItem addOutput:self.videoOutput];

    [self.player.currentItem addObserver:self
                          forKeyPath:@"status"
                             options:NSKeyValueObservingOptionNew
                             context:nil];

    // Seek to the start to render the first frame without playing
    [self.player seekToTime:kCMTimeZero];
    CMTime firstFrameTime = kCMTimeZero;
    [self.videoOutput copyPixelBufferForItemTime:firstFrameTime itemTimeForDisplay:NULL];
    
    self.duration = self.player.currentItem.asset.duration;

    [self setupTimer];
  }
  return self;
}

- (void)dealloc {
  if (self.timer) {
    dispatch_source_cancel(self.timer);
  }
  if (self.texture) {
    SDL_DestroyTexture(self.texture);
    self.texture = nullptr;
  }
  [_player.currentItem removeObserver:self forKeyPath:@"status"];
  [self stop];
  [super dealloc];
}

- (void)observeValueForKeyPath:(NSString *)keyPath
                      ofObject:(id)object
                        change:(NSDictionary<NSKeyValueChangeKey,id> *)change
                       context:(void *)context {
  if ([keyPath isEqualToString:@"status"]) {
    AVPlayerItemStatus status = (AVPlayerItemStatus)[[change objectForKey:NSKeyValueChangeNewKey] integerValue];
    if (status == AVPlayerItemStatusReadyToPlay) {
      LOG(PLATFORM, "Video is ready to play");
    } else if (status == AVPlayerItemStatusFailed) {
      NSError *error = self.player.currentItem.error;
      const char *errorString = [error.localizedDescription UTF8String];
      LOG(PLATFORM, "Video failed to load: %s", errorString);
    }
  } else {
    [super observeValueForKeyPath:keyPath ofObject:object change:change context:context];
  }
}

- (void)play {
  [self.player play];
}

- (void)pause {
  [self.player pause];
}

- (void)stop {
  [self.player pause];
  [self.player seekToTime:kCMTimeZero];
}

- (void)seekToTime:(CMTime)time {
  if (self.isSeeking) {
    LOG(PLATFORM, "A seek operation is already in progress.");
    return;
  }

  CMTime duration = self.player.currentItem.duration;
  if (CMTIME_IS_INVALID(time)) {
    LOG(PLATFORM, "Invalid time.");
    return;
  }

  if (CMTimeCompare(time, duration) > 0) {
    LOG(PLATFORM, "Invalid seek time. Seek time: %.2f seconds, Duration: %.2f seconds", CMTimeGetSeconds(time), CMTimeGetSeconds(duration));
    return;
  }

  if (self.player.status != AVPlayerStatusReadyToPlay) {
    LOG(PLATFORM, "AVPlayer is not ready to play.");
    return;
  }

  if (self.player.currentItem.status != AVPlayerItemStatusReadyToPlay) {
    LOG(PLATFORM, "AVPlayerItem is not ready to play.");
    return;
  }

  self.isSeeking = YES;
  if (!self.isPausedForSeek) {
    self.isPausedForSeek = YES;
    [self.player pause];
  }

  CMTime toleranceBefore = CMTimeMakeWithSeconds(0.5, NSEC_PER_SEC);
  CMTime toleranceAfter = CMTimeMakeWithSeconds(1.0, NSEC_PER_SEC);
  if (CMTimeCompare(time, [self.player currentTime]) < 0) {
      toleranceBefore = CMTimeMakeWithSeconds(1.0, NSEC_PER_SEC);
      toleranceAfter = CMTimeMakeWithSeconds(0.5, NSEC_PER_SEC);
  }

  [self.player seekToTime:time toleranceBefore:toleranceBefore toleranceAfter:toleranceAfter completionHandler:^(BOOL finished) {
    self.isSeeking = NO;
    if (finished) {
      if (self.seekTimer) {
        dispatch_source_cancel(self.seekTimer);
      }
      self.seekTimer = dispatch_source_create(DISPATCH_SOURCE_TYPE_TIMER, 0, 0, dispatch_get_main_queue());
      dispatch_source_set_timer(self.seekTimer, dispatch_time(DISPATCH_TIME_NOW, 250 * NSEC_PER_MSEC), DISPATCH_TIME_FOREVER, 0);
      dispatch_source_set_event_handler(self.seekTimer, ^{
        dispatch_source_cancel(self.seekTimer);
        self.seekTimer = nil;
        self.isPausedForSeek = NO;
        [self.player play];
        LOG(PLATFORM, "Resuming playback after seek.");
      });
      dispatch_resume(self.seekTimer);
    
      CMTime preloadTime = CMTimeAdd(time, CMTimeMakeWithSeconds(0.5, NSEC_PER_SEC));
      CVPixelBufferRef pixelBuffer = [self.videoOutput copyPixelBufferForItemTime:preloadTime itemTimeForDisplay:NULL];
      if (pixelBuffer) {
        dispatch_async(dispatch_get_main_queue(), ^{
          CVBufferRelease(pixelBuffer);
        });
      }
    } else {
      LOG(PLATFORM, "Seek operation failed.");
    }
  }];
}

- (CMTime)getDuration {
  return self.duration;
}

- (CMTime)getCurrentTime {
  return self.player.currentTime;
}

- (void)render:(SDL_Renderer*)renderer inRect:(SDL_Rect&)contentArea {
  if (!self.player || !self.videoOutput) {
    LOG(PLATFORM, "Player or video output not initialized");
    return;
  }

  if (self.player.status != AVPlayerStatusReadyToPlay) {
    LOG(PLATFORM, "AVPlayer not ready to play");
    return;
  }

  if (!self.player.currentItem) {
    LOG(PLATFORM, "Current item is nil");
    return;
  }

  CMTime currentTime = [self.player currentTime];
  CMTime duration = [self.player.currentItem duration];

  CMTime nearEndTime = CMTimeSubtract(duration, CMTimeMakeWithSeconds(0.1, NSEC_PER_SEC));
  if (CMTimeCompare(currentTime, nearEndTime) >= 0) {
    if (self.repeatMode) {
      [self.player seekToTime:kCMTimeZero toleranceBefore:kCMTimeZero toleranceAfter:kCMTimeZero completionHandler:^(BOOL finished) {
        // if (finished) {
        //   LOG(PLATFORM, "Looped video back to start");
        // }
      }];
    } else {
      [self stop];
      return;
    }
  }

  CVPixelBufferRef pixelBuffer = [self.videoOutput copyPixelBufferForItemTime:currentTime itemTimeForDisplay:NULL];

  if (pixelBuffer == nullptr) {
    if (self.texture) {
      SDL_FRect ca;
      ca.x = contentArea.x;
      ca.y = contentArea.y;
      ca.w = contentArea.w;
      ca.h = contentArea.h;
      SDL_RenderTexture(renderer, self.texture, NULL, &ca);
    }
    return;
  }

  CVPixelBufferLockBaseAddress(pixelBuffer, kCVPixelBufferLock_ReadOnly);
  void* baseAddress = CVPixelBufferGetBaseAddress(pixelBuffer);

  size_t width = CVPixelBufferGetWidth(pixelBuffer);
  size_t height = CVPixelBufferGetHeight(pixelBuffer);
  size_t bytesPerRow = CVPixelBufferGetBytesPerRow(pixelBuffer);

  if (!self.texture) {
    self.texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, width, height);
  }

  SDL_UpdateTexture(self.texture, NULL, baseAddress, bytesPerRow);
  SDL_FRect ca;
  ca.x = contentArea.x;
  ca.y = contentArea.y;
  ca.w = contentArea.w;
  ca.h = contentArea.h;
  SDL_RenderTexture(renderer, self.texture, NULL, &ca);

  CVPixelBufferUnlockBaseAddress(pixelBuffer, kCVPixelBufferLock_ReadOnly);
  CVBufferRelease(pixelBuffer);
}

- (void)setLocationCallback:(void (^)(float currentTime, float maxTime))callback {
  if (_locationCallback != callback) {
    _locationCallback = [callback copy];
  }
}

- (void)setRepeatMode:(BOOL)repeatMode {
  _repeatMode = repeatMode;
}

- (void)setupTimer {
  self.timer = dispatch_source_create(DISPATCH_SOURCE_TYPE_TIMER, 0, 0, dispatch_get_main_queue());
  dispatch_source_set_timer(self.timer, DISPATCH_TIME_NOW, 100 * NSEC_PER_MSEC, 0);
  dispatch_source_set_event_handler(self.timer, ^{
    if (self.locationCallback) {
      double currentTime = CMTimeGetSeconds([self.player currentTime]);
      double maxTime = CMTimeGetSeconds(self.duration);
      dispatch_async(dispatch_get_main_queue(), ^{
        self.locationCallback((float)currentTime, (float)maxTime);
      });
    }
  });
  dispatch_resume(self.timer);
}

@end


namespace SGI {
  
  std::string Platform::getUserDocumentDirectory()
  {
    struct passwd *pw = getpwuid(getuid());
    if (!pw) {
      throw std::runtime_error("Failed to get user information");
    }

    std::string homeDir(pw->pw_dir);
    return homeDir + "/Documents";
  }

  Platform::Video::Video(const std::string& absolutePath)
  {
    _osPlayer = [[VideoPlayer alloc] initWithFilePath:absolutePath];
  }

  Platform::Video::~Video()
  {
    [(VideoPlayer*) _osPlayer release];
  }
          
  void Platform::Video::play()
  {
    [(VideoPlayer*) _osPlayer play];
  }

  void Platform::Video::pause()
  {
    [(VideoPlayer*) _osPlayer pause];
  }

  void Platform::Video::stop()
  {
    [(VideoPlayer*) _osPlayer stop];
  }

  void Platform::Video::seek(double seconds)
  {
    CMTime time = CMTimeMakeWithSeconds(seconds, NSEC_PER_SEC);
    [(VideoPlayer*) _osPlayer seekToTime:time];
  }

  double Platform::Video::getDuration() const
  {
    CMTime duration = [(VideoPlayer*) _osPlayer getDuration];
    return CMTimeGetSeconds(duration);
  }

  double Platform::Video::getCurrentTime() const
  {
    CMTime currentTime = [(VideoPlayer*) _osPlayer getCurrentTime];
    return CMTimeGetSeconds(currentTime);
  }

  void Platform::Video::setLocationCallback(std::function<void(float currentTime, float maxTime)> callback) {
    [(VideoPlayer*)_osPlayer setLocationCallback:^(float currentTime, float maxTime) {
      callback(currentTime, maxTime);
    }];
  }

  void Platform::Video::setRepeat(bool value)
  {
    [(VideoPlayer*) _osPlayer setRepeatMode: value];
  }

  void Platform::Video::render(SDL_Renderer* renderer, SDL_Rect& contentArea)
  {
    [(VideoPlayer*) _osPlayer render:renderer inRect:contentArea];
  }

} // namspace SGI
