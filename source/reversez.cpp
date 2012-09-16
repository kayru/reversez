#include <stdio.h>
#include <Rush/Rush.h>
#include <Rush/Platform.h>
#include <Rush/WindowEvents.h>
#include <Rush/GfxPrimitiveBatch.h>
#include <Rush/UtilCamera.h>
#include <Rush/UtilCameraManipulator.h>
#include <Rush/UtilMatrixUtils.h>
#include <Rush/GfxBitmapFont.h>
#include <Rush/MathPlane.h>
#include <Rush/UtilTimer.h>

struct AppData
{
	WindowEvent::KeyDownListener keyboard;

	Camera camera;
	CameraManipulator* camera_man;
	PrimitiveBatch* prim;
	
	BitmapFont* font;
	BlendStateHandle blend_alpha;

	DepthStencilStateHandle depth_off;
	DepthStencilStateHandle depth_fwd;
	DepthStencilStateHandle depth_rev;

	bool show_help;

	Timer frame_timer;
};

void startup(RushPlatformContext* context)
{
	AppData* app = new AppData;
	RushPlatform_SetUserData(context, app);

	RenderDevice* dev = RushPlatform_GetRenderDevice(context);
	Window* win = RushPlatform_GetWindow(context);

	app->keyboard = WindowEvent::KeyDownListener(win);

	app->show_help = true;

	app->prim = new PrimitiveBatch(dev);
	app->font = new BitmapFont(dev, BitmapFont::embedded_font_data());

	const RushAppConfig* cfg = RushPlatform_GetConfig(context);

	float aspect    = float(cfg->width/2) / float(cfg->height);
	float fov       = 1.0f;

	float clip_near = 0.5f;
	float clip_far  = 50000.0f;

	DepthStencilStateDescr depth_desc_off = {CompareFunc::Always, false, false};
	DepthStencilStateDescr depth_desc_fwd = {CompareFunc::LessEqual, true, true};
	DepthStencilStateDescr depth_desc_rev = {CompareFunc::GreaterEqual, true, true};

	app->depth_off = Gfx_DepthStencilState_Create(dev, depth_desc_off);
	app->depth_fwd = Gfx_DepthStencilState_Create(dev, depth_desc_fwd);
	app->depth_rev = Gfx_DepthStencilState_Create(dev, depth_desc_rev);

	app->blend_alpha = Gfx_BlendState_Create(dev, BlendStateDescr::make_alphablend());

	app->camera = Camera(aspect, fov, clip_near, clip_far);    
	app->camera.look_at(Vector3(0, 500, 0), Vector3(0, 500, 500));

	app->camera_man = new CameraManipulator(&app->camera, context);
	app->camera_man->set_turn_speed(1.0f);
	app->camera_man->set_move_speed(1000.0f);
}

void shutdown(RushPlatformContext* context)
{
	AppData* app = (AppData*)RushPlatform_GetUserData(context);
	RenderDevice* dev = RushPlatform_GetRenderDevice(context);
	Gfx_DepthStencilState_Destroy(dev, app->depth_off);
	Gfx_DepthStencilState_Destroy(dev, app->depth_fwd);
	Gfx_DepthStencilState_Destroy(dev, app->depth_rev);
	Gfx_BlendState_Destroy(dev, app->blend_alpha);
	delete app->camera_man;
	delete app->prim;
	delete app->font;
	delete app;
}

void update(RushPlatformContext* context)
{
	AppData* app = (AppData*)RushPlatform_GetUserData(context);
	float dt = app->frame_timer.time();
	app->camera_man->step(dt);

	for( size_t i=0; i<app->keyboard.size(); ++i )
	{
		if( app->keyboard[i].code == 'H' ) 
		{
			app->show_help = !app->show_help;
		}
	}
	app->keyboard.clear();
	app->frame_timer.reset();
}


Vector3 project_vector(const Matrix44& mat, const Vector3& vec)
{
	Vector4 tmp = Vector4(vec, 1);
	tmp = mat.transform_vector(tmp);
	tmp.x /= tmp.w;
	tmp.y /= tmp.w;
	tmp.z /= tmp.w;
	return Vector3(tmp.x, tmp.y, tmp.z);
}

void find_picking_ray(
	const Window* window,
	const Vector3& camera_position,
	const Matrix44& view_proj_inv,
	Vector3* const pos_, Vector3* const dir_ )
{
	Vector2 window_size = Vector2((float)window->width(), (float)window->height());

	Vector2 mouse_pos = window->mouse().pos_pixels / window_size;
	mouse_pos -= Vector2(0.5f, 0.5f);
	mouse_pos *= Vector2(2.0f, -2.0f);

	Vector3 pos = camera_position;
	Vector3 dir = Vector3(mouse_pos.x, mouse_pos.y, 1.0f);

	dir = project_vector(view_proj_inv, dir) - pos;
	dir.normalise_this();

	*pos_ = pos;
	*dir_ = dir;
}

void draw(RushPlatformContext* context)
{
	AppData* app = (AppData*)RushPlatform_GetUserData(context);
	RenderDevice* dev = RushPlatform_GetRenderDevice(context);
	RenderContext* rc = RushPlatform_GetRenderContext(context);
	Window* win = RushPlatform_GetWindow(context);

	Rect window_rect;
	window_rect.left = 0;
	window_rect.top  = 0;
	window_rect.right = win->width();
	window_rect.bottom  = win->height();

	Viewport vp(0, 0, float(win->width()), float(win->height()));

	const Camera& cam = app->camera;
	Matrix44 mat_view = cam.calculate_view();

	Matrix44 mat_proj_fwd = MatrixUtils::perspective(cam.aspect(), cam.fov(), cam.near_plane(), cam.far_plane());
	Matrix44 mat_proj_rev = MatrixUtils::perspective(cam.aspect(), cam.fov(), cam.far_plane(), cam.near_plane());

	struct TestConfig
	{
		DepthStencilStateHandle depth_state;
		Matrix44				proj;
		float					clearz;
		const char*				text;
	};

	TestConfig configs[] = 
	{
		{app->depth_fwd, mat_proj_fwd, 1, "Standard perspective matrix.\nDepth compare function is LessEqual."},
		{app->depth_rev, mat_proj_rev, 0, "Perspective matrix with swapped near and far planes.\nDepth compare function is GreaterEqual."},
	};

	Vector3 v[4] = 
	{
		Vector3(-5000, 0, 0),
		Vector3(-5000, 0, 50000),
		Vector3(+5000, 0, 50000),
		Vector3(+5000, 0, 0),
	};

	ColourRGBA8 c[4] = 
	{
		ColourRGBA8(0x00,0xff,0x00),
		ColourRGBA8(0xff,0x00,0x00),
		ColourRGBA8(0xff,0x00,0xff),
		ColourRGBA8(0x00,0x00,0xff),
	};


	
	Matrix44 view_proj_inv = (mat_view * mat_proj_fwd).inverse();
	Vector3 pick_pos, pick_dir;
	find_picking_ray(win, cam.position(), view_proj_inv, &pick_pos, &pick_dir);

	static int frame_ct = 0;

	Gfx_Context_SetViewport(rc, vp);

	Gfx_Context_SetScissorRect(rc, window_rect);
	Gfx_Context_SetBlendState(rc, app->blend_alpha);
	Gfx_Clear(rc, ColourRGBA8(30,40,50), ClearFlags::Default);

	for( int i=0; i<2; ++i )
	{
		Rect config_scissor;

		config_scissor.left  = (win->width()/2) * i;
		config_scissor.right = (win->width()/2) * (i+1);
		config_scissor.top    = 0;
		config_scissor.bottom = win->height();

		Gfx_Context_SetScissorRect(rc, config_scissor);

		Gfx_Clear(rc, ColourRGBA8(30,40,50), ClearFlags::Depth, configs[i].clearz, 0);
		Gfx_Context_SetDepthStencilState(rc, configs[i].depth_state);

		Matrix44 mat_view_proj = mat_view * configs[i].proj;

		app->prim->begin_3d(mat_view_proj);

		app->prim->draw_triangle(v[0], v[1], v[2], c[0], c[1], c[2]);
		app->prim->draw_triangle(v[2], v[3], v[0], c[2], c[3], c[0]);

		Vector3 offset(0,1,0);
		app->prim->draw_triangle(v[0]-offset, v[1]-offset, v[2]-offset, ColourRGBA8::Black());
		app->prim->draw_triangle(v[2]-offset, v[3]-offset, v[0]-offset, ColourRGBA8::Black());

		for( int j=0; j<=50; ++j )
		{
			app->prim->draw_line(Line3(-6000.0f, 0, j*1000.0f, -5000.0f, 0, j*1000.0f), ColourRGBA8(0xff, 0xff, 0xff, 0x00), ColourRGBA8(0xff, 0xff, 0xff, 0xaa));
			app->prim->draw_line(Line3(+6000.0f, 0, j*1000.0f, +5000.0f, 0, j*1000.0f), ColourRGBA8(0xff, 0xff, 0xff, 0x00), ColourRGBA8(0xff, 0xff, 0xff, 0xaa));
		}

		app->prim->end_3d();

		Gfx_Context_SetDepthStencilState(rc, app->depth_off);
		
		app->prim->begin_2d(win->width(), win->height());
		app->font->draw_string(app->prim, Vector2(config_scissor.left + 5.0f, vp.h-40.0f), configs[i].text);
		app->prim->end_2d();		
	}

	Gfx_Context_SetScissorRect(rc, window_rect);
	Gfx_Context_SetDepthStencilState(rc, app->depth_off);
	app->prim->begin_2d(win->width(), win->height());

	if( app->show_help )
	{
		app->font->draw_string(app->prim, Vector2(10, 10), 
			"About: \n\n"
			" This experiment was written to validate a claim that reversed depth rendering\n"
			" results in improved precision even when used with D24S8 depth buffer.\n"
			" Two quads are rendered on top of each other with 1 unit gap between them. \n"
			" Coloured quad on top of a black one. They are stretched 50k units forward. \n"
			" Camera near clip distance is 0.5, far is 50k.\n"
			" White line markers are drawn every 1k units\n"
			" Trivial vertex shader is used: out_pos = mul(float4(in_pos,1), mat_viewproj);\n"
			" \n"
			"Controls: \n\n"
			" Hold left mouse button to look around.\n"
			" Use WASD+EQ keys to move.\n"
			" Ctrl to slow-down, Shift to speed-up.\n"
			" H to hide or show help text.\n"
		);
	}
	
	char status_str[2048];
	sprintf_s(status_str, "Camera x:%.2f y:%.2f z:%.2f", cam.position().x, cam.position().y, cam.position().z);

	if( pick_dir.dot(Vector3(0,-1,0)) > 0 && cam.position().y > 0 )
	{
		Plane p(0, 1, 0, 0);
		Vector3 cursor = p.intersect_infinite_line(pick_pos, pick_pos + pick_dir);
		sprintf_s(status_str, "%s\nCursor x:%.2f y:%.2f z:%.2f", status_str, cursor.x, cursor.y, cursor.z);
		sprintf_s(status_str, "%s\nDistance to camera: %.2f", status_str, (cam.position()-cursor).length());

		Vector4 fwd = mat_proj_fwd.transform_vector(Vector4(cursor, 1));
		Vector4 rev = mat_proj_rev.transform_vector(Vector4(cursor, 1));

		sprintf_s(status_str, "%s\nForward z:%f w:%f z/w: %f", status_str, fwd.z, fwd.w, fwd.z/fwd.w);
		sprintf_s(status_str, "%s\nReverse z:%f w:%f z/w: %f", status_str, rev.z, rev.w, rev.z/rev.w);
	}

	Vector2 camera_str_size = app->font->measure_string(status_str);
	app->font->draw_string(app->prim, Vector2(float(win->width())-(camera_str_size.x+10), 10), status_str);
	app->prim->draw_line(Line2(win->width()/2.0f,0,win->width()/2.0f,float(win->height())), ColourRGBA8::White());
	app->prim->end_2d();
}

int main()
{
	RushAppConfig cfg;
	RushAppConfig_Init(&cfg);

	cfg.on_startup  = startup;
	cfg.on_shutdown = shutdown;
	cfg.on_draw     = draw;
	cfg.on_update   = update;

	cfg.name = "Reverse Z";

	cfg.width  = 1280;
	cfg.height = 720;

	cfg.max_width  = 2560;
	cfg.max_height = 1600;

	cfg.vsync = 1;

	cfg.resizable = 1;

	return RushPlatform_Run(&cfg);
}

