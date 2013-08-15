
#import <Cocoa/Cocoa.h>
#import <Foundation/Foundation.h>
#import <string>

namespace platform
{

  void messageBox(const char * title, const char * message)
  {
    NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
    NSRunAlertPanel([[[NSString alloc] initWithUTF8String:title] autorelease],
                    [[[NSString alloc] initWithUTF8String:message] autorelease],
                    @"OK", nil, nil);
    [pool drain];
  }

  std::string resourcePath(const char * file)
  {
    #if NDEBUG
      NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];

      std::string dir = "";
      NSBundle * bundle = [NSBundle mainBundle];

      if (bundle == nil)
      {
        dir = newstring("");
      }
      else
      {
        NSString * path = [bundle resourcePath];

        char buf[1024];
        snprintf(buf, 1024, "%s/%s", [path UTF8String], file);
        dir = std::string(buf);
      }

      [pool drain];

      return dir;
    #else
      return std::string(file);
    #endif
  }

}