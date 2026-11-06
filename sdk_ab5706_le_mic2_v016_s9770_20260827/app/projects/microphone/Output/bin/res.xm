loadresdir(res);
makeres(res_buf);
makeresdef(res.h);
xcopy(res.h, ../../res.h);
xcopy(kbox.c, ../../kbox.c);
xcopy(kbox.h, ../../kbox.h);
save(res_buf, res.bin);