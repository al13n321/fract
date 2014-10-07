#include "gl-common.h"

namespace fract {

    class Texture2D{
    public:
        Texture2D(const Texture2D &rhs) = delete;
        Texture2D& operator=(const Texture2D &rhs) = delete;

        Texture2D(int width, int height, GLint internalFormat, GLint filter = GL_NEAREST);
        Texture2D(int width, int height, GLint internalFormat, GLenum format, GLenum type, const GLvoid *data, GLint filter = GL_NEAREST);
        ~Texture2D();

        GLuint name() const { return name_; }
        int width() const { return wid_; }
        int height() const { return hei_; }
        void AssignToUniform(GLint uniform, int unit) const;
        void SetFilter(GLint filter);
        void SetPixels(int x, int y, int width, int height, GLenum format, GLenum type, void *data);
        void SetPixels(GLenum format, GLenum type, void *data);
    protected:
        GLuint name_;
        int wid_, hei_;
    };

}
