//
//  EGLView.h
//  Coffee
//
//  Created by Håvard Bjerke on 24/08/2017.
//
//

#pragma once

#include <CEAGL/eagl_types.h>

#import "../../windowing/glkit/AppDelegate.h"

#include "GLKit/GLKit.h"
#include "GLKit/GLKViewController.h"
#include "GLKit/GLKView.h"

@interface EGLView : NSObject {
    
    
    
}
    + (EGLView*) createView;

    - (bool) createContext: (uint32_t)contextVer;

    - (bool) createView;

    - (void) dealloc;

    - (AppDelegate*) getApp;
    - (EAGLContext*) getContext;
    - (GLKView*) getView;

@end

extern EGLView* current_view;
