#ifndef GLERROR_H
#define GLERROR_H

// PAC Note: this code game from Morten Nobel's blog:
// https://blog.nobel-joergensen.com/2013/01/29/debugging-opengl-using-glgeterror/

void _check_gl_error(const char *file, int line);

///
/// Usage
/// [... some opengl calls]
/// glCheckError();
///
#define check_gl_error() _check_gl_error(__FILE__,__LINE__)

#endif // GLERROR_H