message("Running libmbedcrypto.cmake")

set(mbedtls_path "$ENV{SIMPLELINK_13x2_26x2_SDK}/source/third_party/openthread/third_party/mbedtls/repo/library")
add_library(mbedcrypto STATIC
    "${mbedtls_path}/aes.c"
    "${mbedtls_path}/aesni.c"
    "${mbedtls_path}/arc4.c"
    "${mbedtls_path}/aria.c"
    "${mbedtls_path}/asn1parse.c"
    "${mbedtls_path}/asn1write.c"
    "${mbedtls_path}/base64.c"
    "${mbedtls_path}/bignum.c"
    "${mbedtls_path}/blowfish.c"
    "${mbedtls_path}/camellia.c"
    "${mbedtls_path}/ccm.c"
    "${mbedtls_path}/certs.c"
    "${mbedtls_path}/chacha20.c"
    "${mbedtls_path}/chachapoly.c"
    "${mbedtls_path}/cipher.c"
    "${mbedtls_path}/cipher_wrap.c"
    "${mbedtls_path}/cmac.c"
    "${mbedtls_path}/ctr_drbg.c"
    "${mbedtls_path}/debug.c"
    "${mbedtls_path}/des.c"
    "${mbedtls_path}/dhm.c"
    "${mbedtls_path}/ecdh.c"
    "${mbedtls_path}/ecdsa.c"
    "${mbedtls_path}/ecjpake.c"
    "${mbedtls_path}/ecp.c"
    "${mbedtls_path}/ecp_curves.c"
    "${mbedtls_path}/entropy.c"
    "${mbedtls_path}/entropy_poll.c"
    "${mbedtls_path}/error.c"
    "${mbedtls_path}/gcm.c"
    "${mbedtls_path}/havege.c"
    "${mbedtls_path}/hkdf.c"
    "${mbedtls_path}/hmac_drbg.c"
    "${mbedtls_path}/md.c"
    "${mbedtls_path}/md2.c"
    "${mbedtls_path}/md4.c"
    "${mbedtls_path}/md5.c"
    "${mbedtls_path}/md_wrap.c"
    "${mbedtls_path}/memory_buffer_alloc.c"
    "${mbedtls_path}/net_sockets.c"
    "${mbedtls_path}/nist_kw.c"
    "${mbedtls_path}/oid.c"
    "${mbedtls_path}/padlock.c"
    "${mbedtls_path}/pem.c"
    "${mbedtls_path}/pk.c"
    "${mbedtls_path}/pkcs11.c"
    "${mbedtls_path}/pkcs12.c"
    "${mbedtls_path}/pkcs5.c"
    "${mbedtls_path}/pkparse.c"
    "${mbedtls_path}/pkwrite.c"
    "${mbedtls_path}/pk_wrap.c"
    "${mbedtls_path}/platform.c"
    "${mbedtls_path}/platform_util.c"
    "${mbedtls_path}/poly1305.c"
    "${mbedtls_path}/ripemd160.c"
    "${mbedtls_path}/rsa.c"
    "${mbedtls_path}/rsa_internal.c"
    "${mbedtls_path}/sha1.c"
    "${mbedtls_path}/sha256.c"
    "${mbedtls_path}/sha512.c"
    "${mbedtls_path}/ssl_cache.c"
    "${mbedtls_path}/ssl_ciphersuites.c"
    "${mbedtls_path}/ssl_cli.c"
    "${mbedtls_path}/ssl_cookie.c"
    "${mbedtls_path}/ssl_srv.c"
    "${mbedtls_path}/ssl_ticket.c"
    "${mbedtls_path}/ssl_tls.c"
    "${mbedtls_path}/threading.c"
    "${mbedtls_path}/timing.c"
    "${mbedtls_path}/version.c"
    "${mbedtls_path}/version_features.c"
    "${mbedtls_path}/x509.c"
    "${mbedtls_path}/x509write_crt.c"
    "${mbedtls_path}/x509write_csr.c"
    "${mbedtls_path}/x509_create.c"
    "${mbedtls_path}/x509_crl.c"
    "${mbedtls_path}/x509_crt.c"
    "${mbedtls_path}/x509_csr.c"
    "${mbedtls_path}/xtea.c"
)


target_compile_options(mbedcrypto PRIVATE 
        -mcpu=cortex-m4 
        -march=armv7e-m 
        -mthumb 
        -mfloat-abi=hard 
        -mfpu=fpv4-sp-d16 
        -fno-exceptions 
        -Os 
        -ffunction-sections 
        -fdata-sections 
        -g 
        -gstrict-dwarf 
        -Wall  
        -Wextra 
        -Wshadow 
        -MMD 
        -MP 
        -std=c99 
        $<$<COMPILE_LANGUAGE:CXX>:-Wno-c++14-compat>
)

set(config_file "mbedtls-config-cc1352-gcc.h")
target_compile_definitions(mbedcrypto PRIVATE 
        SIMPLELINK_OPENTHREAD_SDK_BUILD=1 
        SIMPLELINK_OPENTHREAD_CONFIG_CC1352=1 
        MBEDTLS_CONFIG_FILE="${config_file}" 
        HAVE_CONFIG_H 
        _BSD_SOURCE=1 
        _DEFAULT_SOURCE=1
)

target_include_directories(mbedcrypto PUBLIC
    "$ENV{SIMPLELINK_13x2_26x2_SDK}/source/third_party/openthread/examples/platforms"
    "$ENV{SIMPLELINK_13x2_26x2_SDK}/source"
    "$ENV{SIMPLELINK_13x2_26x2_SDK}/source/third_party/openthread/third_party/mbedtls"
    "$ENV{SIMPLELINK_13x2_26x2_SDK}/source/third_party/openthread/third_party/mbedtls/repo/include"
    "$ENV{SIMPLELINK_13x2_26x2_SDK}/source/third_party/openthread/include"
    "$ENV{SIMPLELINK_13x2_26x2_SDK}/kernel/tirtos/packages/gnu/targets/arm/libs/install-native/arm-none-eabi/include/newlib-nano"
    "$ENV{SIMPLELINK_13x2_26x2_SDK}/kernel/tirtos/packages/gnu/targets/arm/libs/install-native/arm-none-eabi/include"
    "$ENV{SIMPLELINK_13x2_26x2_SDK}/source/ti/thread/rtos/${TARGET_BOARD}/thread/libmbedcrypto/config"
)
