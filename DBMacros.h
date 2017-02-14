//
//  DBMacros.h
//  DBCategory
//
//  Created by DreamBuddy on 14/2/17.
//  Copyright © 2017年 DreamBuddy . All rights reserved.
//

#ifndef DBMacros_h
#define DBMacros_h

/* DBLog 解决Xcode8下真机NSLog失效问题 和 在Release模式下屏蔽log */
#if DEBUG
#define DBLog(FORMAT, ...) fprintf(stderr,"%s:%d\t%s\n",[[[NSString stringWithUTF8String:__FILE__] lastPathComponent] UTF8String], __LINE__, [[NSString stringWithFormat:FORMAT, ##__VA_ARGS__] UTF8String]);
#else
#define DBLog(...)
#endif

/* 画一个像素的直线(设计师一般会要求这样纸) */
#define DB_SINGLE_LINE_WIDTH           (1 / [UIScreen mainScreen].scale)
#define DB_SINGLE_LINE_ADJUST_OFFSET   ((1 / [UIScreen mainScreen].scale) / 2)

/* 屏幕尺寸 */
#define DB_SCREEN_WIDTH [UIScreen mainScreen].bounds.size.width
#define DB_SCREEN_HEIGHT [UIScreen mainScreen].bounds.size.height

/* 系统版本 float类型 */
#define DB_IOS_VERSION [[[UIDevice currentDevice] systemVersion] floatValue]

/* Bundle Info.plist */
#define DB_BundleInfoPlist [[NSBundle mainBundle] infoDictionary]
/* Bundle ID */
#define DB_Bundle_ID [[NSBundle mainBundle] bundleIdentifier]
/* Bundle Version */
#define DB_Bundle_Version [DB_BundleInfoPlist objectForKey:@"CFBundleVersion"]
/* Bundle ShortVersion */
#define DB_Bundle_ShortVersion [DB_BundleInfoPlist objectForKey:@"CFBundleShortVersionString"]


/* 
 * 移除警告⚠️ Possible Duplicate: performSelector may cause a leak because its selector is unknown
 * http://stackoverflow.com/questions/11895287/performselector-arc-warning/11895530#11895530
 */
#define DBSuppressPerformSelectorLeakWarning(Stuff) \
do { \
_Pragma("clang diagnostic push") \
_Pragma("clang diagnostic ignored \"-Warc-performSelector-leaks\"") \
Stuff; \
_Pragma("clang diagnostic pop") \
} while (0)

/*
 * 移除警告⚠️ PerformSelector may cause a leak because its selector is unknown
 * http://stackoverflow.com/questions/8773226/performselector-warning
 */
#define DBSuppressPerformSelectorUndeclaredSelector(Stuff) \
do { \
_Pragma ("clang diagnostic push") \
_Pragma ("clang diagnostic ignored \"-Wundeclared-selector\"") \
Stuff; \
_Pragma ("clang diagnostic pop") \
} while (0)

/*
 * 移除警告⚠️ 通用方法
 * 参考：http://www.jianshu.com/p/eb03e20f7b1c 和 http://blog.163.com/smile_lxj/blog/static/2195626200821445250487/
 * Exp: DBSuppressWarning("-Wundeclared-selector",[self performSelector:@selector(crash)];);
 */
#define DBSuppressWarning(Warning,Stuff) \
do { \
_Pragma ("clang diagnostic push") \
DB_Private_DiagnosticIgnore(Warning)\
Stuff; \
_Pragma ("clang diagnostic pop") \
} while (0)
#define DB_Private_DiagnosticIgnore(X) DB_Private_PRAGMA(clang diagnostic ignored X)
#define DB_Private_PRAGMA(X) _Pragma(#X)

/**
 Synthsize a weak or strong reference.
 
 Example:
 @weakify(self)
 [self doSomething^{
 @strongify(self)
 if (!self) return;
 ...
 }];
 
 */
#ifndef weakify
    #if DEBUG
        #if __has_feature(objc_arc)
        #define weakify(object) autoreleasepool{} __weak __typeof__(object) weak##_##object = object;
        #else
        #define weakify(object) autoreleasepool{} __block __typeof__(object) block##_##object = object;
        #endif
    #else
        #if __has_feature(objc_arc)
        #define weakify(object) try{} @finally{} {} __weak __typeof__(object) weak##_##object = object;
        #else
        #define weakify(object) try{} @finally{} {} __block __typeof__(object) block##_##object = object;
        #endif
    #endif
#endif

#ifndef strongify
    #if DEBUG
        #if __has_feature(objc_arc)
        #define strongify(object) autoreleasepool{} __typeof__(object) object = weak##_##object;
        #else
        #define strongify(object) autoreleasepool{} __typeof__(object) object = block##_##object;
        #endif
    #else
        #if __has_feature(objc_arc)
        #define strongify(object) try{} @finally{} __typeof__(object) object = weak##_##object;
        #else
        #define strongify(object) try{} @finally{} __typeof__(object) object = block##_##object;
        #endif
    #endif
#endif

/** 是否为空或是[NSNull null] */
static inline BOOL DBIsNilOrNull(id _ref){
    return (((_ref) == nil) || ([(_ref) isEqual:[NSNull null]]));
}
/** 字符串是否为空 */
static inline BOOL DBIsStringEmpty(id _ref){
    return (((_ref) == nil) || ([(_ref) isEqual:[NSNull null]]) ||([(_ref)isEqualToString:@""]));
}
/** 数组是否为空 */
static inline BOOL DBIsArrayEmpty(id _ref){
    return (((_ref) == nil) || ([(_ref) isEqual:[NSNull null]]) ||([(_ref) count] == 0));
}


/**
 Whether in main queue/thread.
 */
static inline bool db_dispatch_is_main_queue() {
    return pthread_main_np() != 0;
}

/**
 Submits a block for asynchronous execution on a main queue and returns immediately.
 */
static inline void db_dispatch_async_on_main_queue(void (^block)()) {
    if (pthread_main_np()) {
        block();
    } else {
        dispatch_async(dispatch_get_main_queue(), block);
    }
}

/**
 Submits a block for execution on a main queue and waits until the block completes.
 */
static inline void db_dispatch_sync_on_main_queue(void (^block)()) {
    if (pthread_main_np()) {
        block();
    } else {
        dispatch_sync(dispatch_get_main_queue(), block);
    }
}

/**
 *  Instance Method Swizzling
 *
 *  @param originalSelector <#originalSelector description#>
 *  @param swizzledSelector <#swizzledSelector description#>
 *  @param class            <#class description#>
 */
static inline void DBExchangedInstanceMethod(SEL originalSelector, SEL swizzledSelector, Class aClass) {
    Method originalMethod = class_getInstanceMethod(aClass, originalSelector);
    Method swizzledMethod = class_getInstanceMethod(aClass, swizzledSelector);
    BOOL didAddMethod =
    class_addMethod(aClass,
                    originalSelector,
                    method_getImplementation(swizzledMethod),
                    method_getTypeEncoding(swizzledMethod));
    if (didAddMethod) {
        class_replaceMethod(aClass,
                            swizzledSelector,
                            method_getImplementation(originalMethod),
                            method_getTypeEncoding(originalMethod));
    }
    else {
        method_exchangeImplementations(originalMethod, swizzledMethod);
    }
}

/**
 *  Class Method Swizzling
 *
 *  @param originalSelector <#originalSelector description#>
 *  @param swizzledSelector <#swizzledSelector description#>
 *  @param class            <#class description#>
 */
static inline void DBExchangedClassMethod(SEL originalSelector, SEL swizzledSelector, Class aClass) {
    Method originalMethod = class_getInstanceMethod(aClass, originalSel);
    Method newMethod = class_getInstanceMethod(aClass, newSel);
    if (originalMethod && newMethod) {
        method_exchangeImplementations(originalMethod, newMethod);
    }
}

#endif /* DBMacros_h */
