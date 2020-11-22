About
=====
`ngx_progressart_test` is `nginx` example module


Status
======
This module is production-ready.


Configuration directives (same location syntax)
===============================================
progressart_test
-------------------
* **syntax**: `progressart_test on|off`
* **default**: `off`
* **context**: `location`

Allow purging of selected pages from `FastCGI`'s cache.

Sample configuration (same location syntax)
===========================================
    http {
    
        server {
        
            location /test {
                progressart_test    on;
            }
        }
    }


License
=======
    Copyright (c) 2020-2020, PROGRESSART <info@progressart.ru>
    Copyright (c) 2020-2020, Vladimir Smirnov <volodyahome@yandex.ru>
    All rights reserved.
