
#import "cube.h"
#import "tools.h"
#import <Cocoa/Cocoa.h>
#import <Foundation/Foundation.h>

void messagebox(const char * title, const char * message)
{
  NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
  NSRunAlertPanel([[[NSString alloc] initWithUTF8String:title] autorelease], 
                  [[[NSString alloc] initWithUTF8String:message] autorelease],
                  @"OK", nil, nil);
  [pool drain];
}

char * resourcepath(const char * file)
{
  NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
    
  char * dir = 0;
  NSBundle * bundle = [NSBundle mainBundle];
    
  if (bundle == nil)
  {
    dir = newstring("");
  }
  else
  {
    NSString * path = [bundle resourcePath];

    sprintf_sd(str)("%s/%s", [path UTF8String], file);
    dir = newstring(str);
  }
    
  [pool drain];
  
  return dir;
}