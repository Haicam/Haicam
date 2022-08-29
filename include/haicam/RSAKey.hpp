#pragma once

//Replaces any number of consecutive whitespace characters by a single space and remove \r\n
#define MULTILINE(...) #__VA_ARGS__

namespace haicam
{

static const char* SERVER_DEV_RSA2048_PUBLIC_KEY = MULTILINE(-----BEGIN PUBLIC KEY-----
MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEArYa8Tnit5PhCYwrEcf29
LT5kwX9injnTm/CCZiCFr/Eh1sxVM5mjMgmewk5llne6688OA89O/x5AqHf5i5Ba
oPWrIIoTA8uWlQm8KEIh6XzQCPQRK30Gi/nYX2MB2VBsj6O0zitpLOU4l9hOI/Jd
IrVSOZh5MHcIoW3pfT2VPVLmsInjw/e+HmwqT49hawIss4YBucRU8TQcFk+Wao7u
ntm1A6S49ha+x0KjyH8tbi2awhcuJ9clWooov6v5y4BWJ7HBSSmLg992EZeiv+Fm
BsguFgiFrNGW/rMyOPu5OjehlkrQeK2hVm6eRpE+HHVPVrft6+XiafAYjLRzal5p
gQIDAQAB
-----END PUBLIC KEY-----);

static const char* SERVER_RSA2048_PUBLIC_KEY = MULTILINE(-----BEGIN PUBLIC KEY-----
MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEArYa8Tnit5PhCYwrEcf29
LT5kwX9injnTm/CCZiCFr/Eh1sxVM5mjMgmewk5llne6688OA89O/x5AqHf5i5Ba
oPWrIIoTA8uWlQm8KEIh6XzQCPQRK30Gi/nYX2MB2VBsj6O0zitpLOU4l9hOI/Jd
IrVSOZh5MHcIoW3pfT2VPVLmsInjw/e+HmwqT49hawIss4YBucRU8TQcFk+Wao7u
ntm1A6S49ha+x0KjyH8tbi2awhcuJ9clWooov6v5y4BWJ7HBSSmLg992EZeiv+Fm
BsguFgiFrNGW/rMyOPu5OjehlkrQeK2hVm6eRpE+HHVPVrft6+XiafAYjLRzal5p
gQIDAQAB
-----END PUBLIC KEY-----);

}