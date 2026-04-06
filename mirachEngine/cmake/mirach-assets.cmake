# This file should provide rules for importing asset files.
# NOTE: This file is a work in progress.
# TODO: Implement function to get list of assets with specified tag.

file(GLOB MIRACH_GAME_ASSETS "${MIRACH_GAME_DIR}/*")


if(MIRACH_GAME_EMBED_DEFAULT)
    set(MIRACH_GAME_EMBED_ASSETS "")
    set(MIRACH_GAME_EXTERN_ASSETS "")
    mirach_get_assets("-extern")

else()


endif()



function(mirach_get_assets tag)
    # do stuff

endfunction()