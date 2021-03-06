## Very useful for debugging purposes: Disables the dependency on the "make install" target.
## In our case e.g. "make install" always builds the documentation etc. 
#set(CMAKE_SKIP_INSTALL_ALL_DEPENDENCY On)

set(CPACK_GENERATOR "DragNDrop")

## We want to package the whole top-level dir so a user can drag'n'drop it via the image.
set(CPACK_INCLUDE_TOPLEVEL_DIRECTORY 0) ## dmg seems to be component-aware and makes an ALL-IN-ONE package
set(CPACK_COMPONENT_INCLUDE_TOPLEVEL_DIRECTORY 1) ## Therefore _only_ use the second.. weird stuff.
## we make sure it is called like we want although this is the standard name I think.
set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${SMARTPEAK_PACKAGE_VERSION_FULLSTRING}-macOS")  

## Fix SmartPeak dependencies for all executables in the install directory under bin.
## That affects everything but the bundles (which have their own structure and are fixed up in add_mac_bundle.cmake)
########################################################### Fix Dependencies
install(CODE "execute_process(COMMAND ${PROJECT_SOURCE_DIR}/cmake/MacOSX/fix_dependencies.rb -b \${CMAKE_INSTALL_PREFIX}/${INSTALL_BIN_DIR}/ -l \${CMAKE_INSTALL_PREFIX}/${INSTALL_LIB_DIR}/ )"
  COMPONENT zzz-fixing-dependencies
)

install(CODE "execute_process(COMMAND ${PROJECT_SOURCE_DIR}/cmake/MacOSX/sign_bins_and_libs.rb -d \${CMAKE_INSTALL_PREFIX}/ -s ${CPACK_BUNDLE_APPLE_CERT_APP})"
  COMPONENT zzz-sign-bins-and-libs
)

## Create own target because you cannot "depend" on the internal target 'package'
add_custom_target(dist
  COMMAND cpack -G ${CPACK_GENERATOR}
  COMMENT "Building ${CPACK_GENERATOR} package"
)

########################################################### Create dmg with background image
set(SMARTPEAK_LOGO ${PROJECT_SOURCE_DIR}/cmake/MacOSX/openms_logo_large_transparent.png) ## The logo to be used for the SmartPeak folder on the DMG
set(SMARTPEAK_DMG_FOLDER_NAME "${CPACK_PACKAGE_NAME}-${SMARTPEAK_PACKAGE_VERSION_FULLSTRING}") ## The name of the SmartPeak folder on the DMG
configure_file(${PROJECT_SOURCE_DIR}/cmake/MacOSX/setup_applescript.scpt.in ${PROJECT_BINARY_DIR}/macOS_bundle_setup/setup_applescript.scpt)
set(CPACK_DMG_DS_STORE_SETUP_SCRIPT ${PROJECT_BINARY_DIR}/macOS_bundle_setup/setup_applescript.scpt)
#Next line could overcome a script but since we do not have a fixed name of the SmartPeak-$VERSION folder, it probably won't work
#set(CPACK_DMG_DS_STORE ${PROJECT_SOURCE_DIR}/cmake/MacOSX/DS_store_new)
set(CPACK_DMG_BACKGROUND_IMAGE ${PROJECT_SOURCE_DIR}/cmake/MacOSX/background.png)
set(CPACK_DMG_FORMAT UDBZ) ## Try bzip2 to get slighlty smaller images

## Sign the image. CPACK_BUNDLE_APPLE_CERT_APP needs to be unique and found in one of the
## keychains in the search list (which needs to be unlocked).
if (DEFINED CPACK_BUNDLE_APPLE_CERT_APP)
add_custom_target(signed_dist
  COMMAND codesign --deep --force --sign ${CPACK_BUNDLE_APPLE_CERT_APP} ${CPACK_PACKAGE_FILE_NAME}.dmg
  COMMAND ${SMARTPEAK_HOST_DIRECTORY}/cmake/MacOSX/notarize_app.sh ${CPACK_PACKAGE_FILE_NAME}.dmg com.smartpeak ${SIGNING_EMAIL} CODESIGNPW ${SMARTPEAK_HOST_BINARY_DIRECTORY}
  WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
  COMMENT "Signing and notarizing ${CPACK_PACKAGE_FILE_NAME}.dmg as ${CPACK_BUNDLE_APPLE_CERT_APP}"
  DEPENDS dist)
endif()

