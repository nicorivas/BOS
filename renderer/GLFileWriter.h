/* 
 * File:   GLFileWriter.h
 * Author: dducks
 *
 * Created on May 11, 2015, 11:33 AM
 */

#ifndef GLFILEWRITER_H
#define	GLFILEWRITER_H

#ifdef ENABLE_VIDEO
extern "C" {
#include <libpng/png.h>
#include <libavcodec/avcodec.h>
#include <libavutil/common.h>
#include <libavutil/imgutils.h>
#include <libavutil/samplefmt.h>
#include <libavutil/mathematics.h>
#include <libavutil/channel_layout.h>
}
    
#include <GL/glew.h>

#include "Camera.h"

#include <fstream>
#include <iostream>

class GLOffscreenBuffer {
    uint8_t * buffer;
    std::size_t width, height;
    
    GLuint fbo[2];
    GLuint tex[1];
    GLuint rbo[2];
    
    float fov, zfar, znear;
    
    Matrix<4,4,GLfloat> proj_Storage;
    unsigned int vpW, vpH;
    
public:
    GLOffscreenBuffer(float fov, float zfar, float znear, std::size_t width, std::size_t height)
       : width(width), height(height), fov(fov), zfar(zfar), znear(znear)
    {
        buffer = new uint8_t[width * height * 4];
        glGenFramebuffers(2,fbo);
        glGenRenderbuffers(2,rbo);
        
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo[0]);
        
        //glGenTextures(1, tex);
        //glBindTexture( GL_TEXTURE_2D_MULTISAMPLE, tex[0] );
        //glTexImage2DMultisample( GL_TEXTURE_2D_MULTISAMPLE, 16, GL_RGBA, width, height, GL_TRUE );
        //glBindTexture( GL_TEXTURE_2D_MULTISAMPLE, 0);
        //glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, tex[0], 0);
        

        
        glBindRenderbuffer(GL_RENDERBUFFER, rbo[0]);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA8, width, height);
        //glRenderbufferStorageMultisample(GL_RENDERBUFFER, 16, GL_RGBA8, width, height);
        
        glBindRenderbuffer(GL_RENDERBUFFER, rbo[1]);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, width, height);
        //glRenderbufferStorageMultisample(GL_RENDERBUFFER, 16, GL_DEPTH_COMPONENT24, width, height);
        
        
        
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, rbo[0]);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo[1]);
        
        //glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex[1], 0);
        //glFramebufferTexture2D( GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D_MULTISAMPLE, tex, 0);
        
        
        
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    }
       
    GLOffscreenBuffer(const GLOffscreenBuffer& other) = delete;
    GLOffscreenBuffer(GLOffscreenBuffer&& other) = default;
    GLOffscreenBuffer& operator=(const GLOffscreenBuffer& other) = delete;
    GLOffscreenBuffer& operator=(GLOffscreenBuffer&& other) = default;
       
    ~GLOffscreenBuffer() {
        delete [] buffer;
        glDeleteFramebuffers(2, fbo);
        glDeleteRenderbuffers(2, rbo);
        glDeleteTextures(1, tex);
    }
    
    void startRender(Camera& cam) {
        proj_Storage = cam.getProjectionMatrix();
        vpW = cam.getViewportWidth();
        vpH = cam.getViewportHeight();
        glBindFramebuffer(GL_FRAMEBUFFER, fbo[0]);
        
        cam.setProjection(fov, zfar, znear, width, height);
        glEnable( GL_DEPTH_TEST );
        glEnable( GL_MULTISAMPLE );
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }
    
    void stopRender(Camera& cam) {

        glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo[0]);
        
        glReadBuffer(GL_COLOR_ATTACHMENT0);
        
        glReadPixels(0,0,width,height,GL_RGBA, GL_UNSIGNED_BYTE, buffer);
        
        cam.setViewport(vpW, vpH);
        cam.setProjectionMatrix(proj_Storage);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
               
    }
    
    std::size_t getWidth() const {
        return width;
    }
    
    std::size_t getHeight() const {
        return height;
    }
    
    const uint8_t * data() const {
        return buffer;
    }
};

class GLImageCodec {
public:
    
};

static bool registeredCodecs = false;

class GLVideoCodec {
    
    AVCodec * codec;
    AVCodecContext * c;
    std::ofstream file;
    AVFrame * frame;
    AVPacket pkt;
    bool closed;
public:
    GLVideoCodec(std::string filename, const GLOffscreenBuffer& buffer, double timestep)
        : file(filename, std::ios_base::binary | std::ios_base::trunc | std::ios_base::out), closed(false)
    {
        if (!registeredCodecs) {
            registeredCodecs = true;
            avcodec_register_all();
        }
        codec = avcodec_find_encoder(AV_CODEC_ID_H264);
        if (!codec) {
            std::cerr << "Codec not found" << std::endl;
            return;
        }
        c = avcodec_alloc_context3(codec);
        if (!c) {
            std::cerr << "Context failure" << std::endl;
        }
        c->bit_rate = 0;//100 * buffer.getWidth() * buffer.getHeight();
        c->width = buffer.getWidth();
        c->height = buffer.getHeight();
        c->time_base = (AVRational){1, 25};
        c->gop_size = 10;
        c->max_b_frames = 1;
        c->pix_fmt =  AV_PIX_FMT_YUV444P;
        
        //av_opt_set(c->priv_data, "preset", "slow", 0);
        
        if (avcodec_open2(c, codec, NULL) < 0) {
            std::cerr << "Could not open codec" << std::endl;
            return;
        }
        
        frame = avcodec_alloc_frame();
        frame->format = c->pix_fmt; //AV_PIX_FMT_YUV444P;
        frame->width = c->width;
        frame->height = c->height;
        frame->pts = 0;
        if (av_image_alloc(frame->data, frame->linesize, c->width, c->height, c->pix_fmt, 32) < 0) {
            std::cerr << "Couldn't allocate picture buffer" << std::endl;
            return;
        }
        
    }
        
    ~GLVideoCodec() {
        close();
    }
        
    void writeFrame(const GLOffscreenBuffer& buff) {
        int outputAvailable = 0;
        av_init_packet(&pkt);
        pkt.data = NULL;
        pkt.size = 0;
        
        for (std::size_t x = 0; x < buff.getWidth(); x++) {
            for (std::size_t y = 0; y < buff.getHeight(); y++) {
                
                uint8_t r, g, b;
                r = buff.data()[4*((buff.getHeight() - y - 1) * buff.getWidth() + x) + 0];
                g = buff.data()[4*((buff.getHeight() - y - 1) * buff.getWidth() + x) + 1];
                b = buff.data()[4*((buff.getHeight() - y - 1) * buff.getWidth() + x) + 2];
                
                double y_ = 16  +  0.257 * r +  0.504 * g +  0.098 * b;
                double cb = 128 + -0.148 * r + -0.291 * g +  0.439 * b;
                double cr = 128 +  0.439 * r + -0.368 * g + -0.071 * b;
                
                frame->pts++;
                
                frame->data[0][y * frame->linesize[0] + x] = (uint8_t)y_;// buff.data()[4*(y * buff.getWidth() + x) + 2];
                frame->data[1][y * frame->linesize[1] + x] = (uint8_t)cb;// buff.data()[4*(y * buff.getWidth() + x) + 0];
                frame->data[2][y * frame->linesize[2] + x] = (uint8_t)cr; //buff.data()[4*(y * buff.getWidth() + x) + 0];
                
                //frame->data[3][y * frame->linesize[3] + x] = buff.data()[4*(y * buff.getWidth() + x) + 3];
            }
        }
        
        if (avcodec_encode_video2(c, &pkt, frame, &outputAvailable) < 0) {
            std::cerr << "Frame writing failure" << std::endl;
        }
        if (outputAvailable) {
            std::cout << "Frame size=" << pkt.size << std::endl;
            file.write((char*)pkt.data,pkt.size);
            av_free_packet(&pkt);
        }
    }
    
    void close() {
        if (closed)
            return;
        closed = true;
        int outputAvailable;
        do {
            avcodec_encode_video2(c, &pkt, NULL, &outputAvailable);
            
            if (outputAvailable) {
                file.write((char*)pkt.data, pkt.size);
                av_free_packet(&pkt);
            }
        } while (outputAvailable);
        uint8_t data[] = {0,0,1,0xb7};
        file.write((char*)data,4);
        file.flush();
        file.close();
        avcodec_close(c);
        av_free(c);
        av_freep(&frame->data[0]);
        avcodec_free_frame(&frame);
        std::cout << "Done." << std::endl;
    }
    
};

#endif  /* ENABLE_VIDEO */
#endif	/* GLFILEWRITER_H */

