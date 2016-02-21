diff --git a/CMakeLists.txt b/CMakeLists.txt
index c754196..7c4083c 100644
--- a/CMakeLists.txt
+++ b/CMakeLists.txt
@@ -1231,7 +1231,7 @@ if(VRPN_BUILD_SERVER_LIBRARY)
 	add_library(vrpnserver
 		${VRPN_SERVER_SOURCES}
 		${VRPN_SERVER_PUBLIC_HEADERS})
-	target_link_libraries(vrpnserver ${EXTRA_LIBS} ${SERVER_EXTRA_LIBS})
+	target_link_libraries(vrpnserver ${EXTRA_LIBS} ${SERVER_EXTRA_LIBS} dl)
 	set(VRPN_CLIENT_LIBRARY vrpnserver)
 	set(VRPN_SERVER_LIBRARY vrpnserver)
 	set_property(TARGET
@@ -1282,7 +1282,7 @@ endif()
 
 if(VRPN_BUILD_CLIENT_LIBRARY)
 	add_library(vrpn ${VRPN_CLIENT_SOURCES} ${VRPN_CLIENT_PUBLIC_HEADERS})
-	target_link_libraries(vrpn ${EXTRA_LIBS})
+	target_link_libraries(vrpn ${EXTRA_LIBS} dl)
 	set(VRPN_CLIENT_LIBRARY vrpn)
 
 	set_property(TARGET
diff --git a/cmake/FindOVR.cmake b/cmake/FindOVR.cmake
index fa1473f..b666601 100644
--- a/cmake/FindOVR.cmake
+++ b/cmake/FindOVR.cmake
@@ -90,6 +90,8 @@ find_library(OVR_LIBRARY_RELEASE
 	NAMES
 	ovr${_ovr_libname_bitsuffix}
 	libovr${_ovr_libname_bitsuffix}
+	OVR
+	libOVR
 	PATHS
 	"${OVR_ROOT_DIR}"
 	"${OVR_ROOT_DIR}/LibOVR"
@@ -103,6 +105,8 @@ find_library(OVR_LIBRARY_DEBUG
 	NAMES
 	ovr${_ovr_libname_bitsuffix}d
 	libovr${_ovr_libname_bitsuffix}d
+	OVR
+	libOVR
 	PATHS
 	"${OVR_ROOT_DIR}"
 	"${OVR_ROOT_DIR}/LibOVR"
