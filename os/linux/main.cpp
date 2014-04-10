
// main.cpp - Outlaws.h platform implementation for linux (together with sdl_os.cpp)

#include "StdAfx.h"
#include "../sdl_os/sdl_os.h"

#include <ftw.h>
#include <unistd.h>

#include <libgen.h>
#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>

char *g_binaryName;

static const string& getDataDir()
{
    static string datadir;
    if (datadir.empty())
    {
        datadir = g_binaryName;
        datadir = dirname((char*)datadir.c_str());
        datadir += "/../../../";
        char buf1[256];
        datadir = realpath(datadir.c_str(), buf1);
        datadir += "/";
    }
    return datadir;
}

const char *OL_PathForFile(const char *fname, const char* mode)
{
    ASSERT(fname);

    // absolute path
    if (!fname || fname[0] == '/')
        return fname;

    // FIXME store save in dotfile?

    static string path;
    path = getDataDir();
    path += fname;
    return path.c_str();
}

static void recursive_mkdir(const char *dir)
{
    char tmp[256];
    char *p = NULL;
    size_t len;

    snprintf(tmp, sizeof(tmp),"%s",dir);
    len = strlen(tmp);

    if(tmp[len - 1] == '/')
        tmp[len - 1] = 0;
    for(p = tmp + 1; *p; p++)
    {
        if(*p == '/')
        {
            *p = 0;
            mkdir(tmp, S_IRWXU);
            *p = '/';
        }
    }
    mkdir(tmp, S_IRWXU);
}

static void create_intermediate_directories(const char* path)
{
    char tmp[256];
    snprintf(tmp, sizeof(tmp),"%s", path);

    const char* dir = dirname(tmp);
    recursive_mkdir(dir);
}

int OL_SaveFile(const char *name, const char* data)
{
    const char* fname = OL_PathForFile(name, "w");
    create_intermediate_directories(fname);
    
    string fnameb = string(fname) + ".b";

    FILE *f = fopen(fnameb.c_str(), "w");
    if (!f)
    {
        ReportMessagef("error opening '%s' for writing\n", fnameb.c_str());
        return 0;
    }
    int bytesWritten = fprintf(f, "%s", data);
    if (bytesWritten != strlen(data))
    {
        ReportMessagef("writing to '%s', wrote %d bytes of expected %d\n", fnameb.c_str(), bytesWritten, strlen(data));
        return 0;
    }
    if (fclose(f) != 0)
    {
        ReportMessagef("error closing temp file from '%s': %s'\n", fnameb.c_str(), strerror(errno));
        return 0;
    }
    
    if (rename(fnameb.c_str(), fname) != 0)
    {
        ReportMessagef("error renaming temp file from '%s' to '%s': %s'\n", fnameb.c_str(), fname, strerror(errno));
        return 0;
    }

    return 1;
}

int unlink_cb(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf)
{
    int rv = remove(fpath);

    if (rv) {
        ReportMessagef("Error removing '%s'", fpath, strerror(errno));
    }

    return rv;
}

int OL_RemoveFileOrDirectory(const char* dirname)
{
    const char *path = OL_PathForFile(dirname, "r");
    nftw(path, unlink_cb, 64, FTW_DEPTH | FTW_PHYS);
    return 1;
}

int OL_FileDirectoryPathExists(const char* fname)
{
    const char *path = OL_PathForFile(fname, "r");
    struct stat buf;
    if (stat(path, &buf)) {
        ReportMessagef("Error stating path: '%s'", strerror(errno));
        return 0;
    }

    return S_ISREG(buf.st_mode) || S_ISDIR(buf.st_mode);
}

const char *OL_LoadFile(const char *name)
{
    const char *fname = OL_PathForFile(name, "r");
    int bufsize = 257;
    char *buffer = (char *) malloc(bufsize);
    FILE *f = fopen(fname, "r");
    if (!f)
    {
        //ReportMessagef("error opening '%s' for reading\n", fname);
        return NULL;
    }
    int c, indx=0;
    while ((c = fgetc(f)) != EOF) {
		if (c != '\r')
			buffer[indx++] = (char) c;
		if (indx >= bufsize) {
            bufsize *= 2;
            buffer = (char *) realloc(buffer, bufsize);
        }
    }
    fclose(f);
    buffer[indx++] = '\0';

    buffer = (char *) realloc(buffer, indx);
    return buffer;
}



int main(int argc, char** argv)
{
    g_binaryName = argv[0];

    return sdl_os_main(argc, argv);
}