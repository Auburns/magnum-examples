/*
    This file is part of Magnum.

    Original authors — credit is appreciated but not required:

        2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018 —
            Vladimír Vondruš <mosra@centrum.cz>

    This is free and unencumbered software released into the public domain.

    Anyone is free to copy, modify, publish, use, compile, sell, or distribute
    this software, either in source code form or as a compiled binary, for any
    purpose, commercial or non-commercial, and by any means.

    In jurisdictions that recognize copyright laws, the author or authors of
    this software dedicate any and all copyright interest in the software to
    the public domain. We make this dedication for the benefit of the public
    at large and to the detriment of our heirs and successors. We intend this
    dedication to be an overt act of relinquishment in perpetuity of all
    present and future rights to this software under copyright law.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
    THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
    IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
    CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include <Corrade/PluginManager/Manager.h>
#include <Magnum/AbstractShaderProgram.h>
#include <Magnum/Buffer.h>
#include <Magnum/Context.h>
#include <Magnum/CubeMapTexture.h>
#include <Magnum/DefaultFramebuffer.h>
#include <Magnum/Extensions.h>
#include <Magnum/Mesh.h>
#include <Magnum/Renderer.h>
#include <Magnum/Texture.h>
#include <Magnum/Platform/Sdl2Application.h>
#include <Magnum/SceneGraph/Drawable.h>
#include <Magnum/SceneGraph/Scene.h>
#include <Magnum/SceneGraph/Camera.h>
#include <Magnum/Trade/AbstractImporter.h>

#include "CubeMap.h"
#include "Reflector.h"
#include "Types.h"
#include "configure.h"

#ifdef MAGNUM_BUILD_STATIC
/* Import plugins in static build */
static int importStaticPlugins() {
    CORRADE_PLUGIN_IMPORT(StbImageImporter)
    return 0;
} CORRADE_AUTOMATIC_INITIALIZER(importStaticPlugins)
#endif

namespace Magnum { namespace Examples {

class CubeMapExample: public Platform::Application {
    public:
        explicit CubeMapExample(const Arguments& arguments);

    private:
        void viewportEvent(const Vector2i& size) override;
        void drawEvent() override;
        void keyPressEvent(KeyEvent& event) override;
        void mousePressEvent(MouseEvent& event) override;
        void mouseMoveEvent(MouseMoveEvent& event) override;

        CubeMapResourceManager _resourceManager;
        Scene3D _scene;
        SceneGraph::DrawableGroup3D _drawables;
        Object3D* _cameraObject;
        SceneGraph::Camera3D* _camera;

        Vector2i _previousMousePosition;
};

CubeMapExample::CubeMapExample(const Arguments& arguments): Platform::Application(arguments, Configuration().setTitle("Magnum Cube Map Example").setWindowFlags(Configuration::WindowFlag::Resizable
    #ifdef CORRADE_TARGET_IOS
    |Configuration::WindowFlag::Borderless|Configuration::WindowFlag::AllowHighDpi
    #endif
    ))
{
    Renderer::enable(Renderer::Feature::DepthTest);
    Renderer::enable(Renderer::Feature::FaceCulling);

    /* Set up perspective camera */
    (_cameraObject = new Object3D(&_scene))
        ->translate(Vector3::zAxis(3.0f));
    (_camera = new SceneGraph::Camera3D(*_cameraObject))
        ->setAspectRatioPolicy(SceneGraph::AspectRatioPolicy::Extend)
        .setProjectionMatrix(Matrix4::perspectiveProjection(Deg(55.0f), 1.0f, 0.001f, 100.0f))
        .setViewport(defaultFramebuffer.viewport().size());

    /* Load TGA importer plugin */
    PluginManager::Manager<Trade::AbstractImporter> manager(MAGNUM_PLUGINS_IMPORTER_DIR);
    std::unique_ptr<Trade::AbstractImporter> importer = manager.loadAndInstantiate("JpegImporter");
    if(!importer) std::exit(1);

    _resourceManager.set<Trade::AbstractImporter>("jpeg-importer",
        importer.release(), ResourceDataState::Final, ResourcePolicy::Manual);

    /* Add objects to scene */
    (new CubeMap(arguments.argc == 2 ? arguments.argv[1] : "", &_scene, &_drawables))
        ->scale(Vector3(20.0f));

    (new Reflector(&_scene, &_drawables))
        ->scale(Vector3(0.5f))
        .translate(Vector3::xAxis(-0.5f));

    (new Reflector(&_scene, &_drawables))
        ->scale(Vector3(0.3f))
        .rotate(Deg(37.0f), Vector3::xAxis())
        .translate(Vector3::xAxis(0.3f));

    /* We don't need the importer anymore */
    _resourceManager.free<Trade::AbstractImporter>();
}

void CubeMapExample::viewportEvent(const Vector2i& size) {
    defaultFramebuffer.setViewport({{}, size});
    _camera->setViewport(size);
}

void CubeMapExample::drawEvent() {
    defaultFramebuffer.clear(FramebufferClear::Depth);
    defaultFramebuffer.invalidate({DefaultFramebuffer::InvalidationAttachment::Color});

    _camera->draw(_drawables);
    swapBuffers();
}

void CubeMapExample::keyPressEvent(KeyEvent& event) {
    if(event.key() == KeyEvent::Key::Up)
        _cameraObject->rotate(Deg(-10.0f), _cameraObject->transformation().right().normalized());

    else if(event.key() == KeyEvent::Key::Down)
        _cameraObject->rotate(Deg(10.0f), _cameraObject->transformation().right().normalized());

    else if(event.key() == KeyEvent::Key::Left || event.key() == KeyEvent::Key::Right) {
        Float translationY = _cameraObject->transformation().translation().y();
        _cameraObject->translate(Vector3::yAxis(-translationY))
            .rotateY(event.key() == KeyEvent::Key::Left ? Deg(10.0f) : Deg(-10.0f))
            .translate(Vector3::yAxis(translationY));

    } else return;

    redraw();
}

void CubeMapExample::mousePressEvent(MouseEvent& event) {
    #ifndef CORRADE_TARGET_ANDROID
    if(event.button() != MouseEvent::Button::Left) return;
    #endif

    _previousMousePosition = event.position();
    event.setAccepted();
}

void CubeMapExample::mouseMoveEvent(MouseMoveEvent& event) {
    #ifndef CORRADE_TARGET_ANDROID
    if(!(event.buttons() & MouseMoveEvent::Button::Left)) return;
    #endif

    const Vector2 delta = 3.0f*
        Vector2{event.position() - _previousMousePosition}/
        Vector2{defaultFramebuffer.viewport().size()};

    (*_cameraObject).rotate(Rad{delta.y()}, _cameraObject->transformation().right().normalized());

    Float translationY = _cameraObject->transformation().translation().y();
    _cameraObject->translate(Vector3::yAxis(-translationY))
        .rotateY(Rad{delta.x()})
        .translate(Vector3::yAxis(translationY));

    _previousMousePosition = event.position();
    event.setAccepted();
    redraw();
}

}}

MAGNUM_APPLICATION_MAIN(Magnum::Examples::CubeMapExample)
