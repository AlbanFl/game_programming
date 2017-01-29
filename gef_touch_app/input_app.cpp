#include "input_app.h"
#include <system/platform.h>
#include <graphics/sprite_renderer.h>
#include <graphics/font.h>
#include <input/input_manager.h>
#include <input/touch_input_manager.h>
#include <system/debug_log.h>
#include <vector>
#include <gef.h>
#include <maths/vector2.h>


InputApp::InputApp(gef::Platform& platform) :
	Application(platform),
	sprite_renderer_(NULL),
	font_(NULL),
	input_manager_(NULL),
	active_touch_id_(-1)
{
}

void InputApp::Init()
{
	sprite_renderer_ = platform_.CreateSpriteRenderer();
	input_manager_ = platform_.CreateInputManager();

	sprite1.set_position(480.0f, 272.0f, 0.0f);
	sprite1.set_width(32.0f);
	sprite1.set_height(32.0f);

	// make sure if there is a panel to detect touch input, then activate it
	if (input_manager_ && input_manager_->touch_manager() && (input_manager_->touch_manager()->max_num_panels() > 0))
		input_manager_->touch_manager()->EnablePanel(0);

	InitFont();
}

void InputApp::CleanUp()
{
	CleanUpFont();

	delete input_manager_;
	input_manager_ = NULL;

	delete sprite_renderer_;
	sprite_renderer_ = NULL;
}

bool InputApp::Update(float frame_time)
{
	fps_ = 1.0f / frame_time;

	if (input_manager_)
	{
		input_manager_->Update();

		ProcessTouchInput();
	}

	

	return true;
}

void InputApp::Render()
{
	sprite_renderer_->Begin();
	sprite_renderer_->DrawSprite(sprite1);
	DrawHUD();
	sprite_renderer_->End();
}
void InputApp::InitFont()
{
	font_ = new gef::Font(platform_);
	font_->Load("comic_sans");
}

void InputApp::CleanUpFont()
{
	delete font_;
	font_ = NULL;
}

void InputApp::DrawHUD()
{
	if(font_)
	{
		// if a touch is active lets draw some text
		if (active_touch_id_ != -1)
		{
			font_->RenderText(sprite_renderer_, gef::Vector4(touch_position_.x, touch_position_.y, -0.9f), 1.0f, 0xffffffff, gef::TJ_LEFT, "(%.1f, %.1f)", touch_position_.x, touch_position_.y);
		}

		// display frame rate
		font_->RenderText(sprite_renderer_, gef::Vector4(850.0f, 510.0f, -0.9f), 1.0f, 0xffffffff, gef::TJ_LEFT, "FPS: %.1f", fps_);
	}
}

void InputApp::ProcessTouchInput()
{
	const gef::TouchInputManager* touch_input = input_manager_->touch_manager();
	if (touch_input && (touch_input->max_num_panels() > 0))
	{
		// get the active touches for this panel
		const gef::TouchContainer& panel_touches = touch_input->touches(0);

		// go through the touches
		for (gef::ConstTouchIterator touch = panel_touches.begin(); touch != panel_touches.end(); ++touch)
		{
			// if active touch id is -1, then we are not currently processing a touch
			if (active_touch_id_ == -1)
			{
				// check for the start of a new touch
				if (touch->type == gef::TT_NEW)
				{
					active_touch_id_ = touch->id;

					// do any processing for a new touch here
					// we're just going to record the position of the touch
					touch_position_ = touch->position;
					sprite1.set_position(touch_position_.x, touch_position_.y, 0);
				}
			}
			else if (active_touch_id_ == touch->id)
			{
				// we are processing touch data with a matching id to the one we are looking for
				if (touch->type == gef::TT_ACTIVE)
				{
					// update an active touch here
					// we're just going to record the position of the touch
					touch_position_ = touch->position;
					sprite1.set_colour(0xFF0000FF);
				}
				else if (touch->type == gef::TT_RELEASED)
				{
					// the touch we are tracking has been released
					// perform any actions that need to happen when a touch is released here
					// we're not doing anything here apart from resetting the active touch id
					sprite1.set_colour(0xFF00FF00);
					/*touch_position_ = touch->position;
					gef::Vector4 sprite_position = sprite1.position();

					gef::Vector2 start_pos = (sprite_position.x(), sprite_position.y());
					gef::Vector2 my_vect = touch_position_ - start_pos;
					my_vect = my_vect / fps_;
					start_pos = start_pos + my_vect;
					sprite1.set_position(start_pos.x, start_pos.y, 0);*/
					active_touch_id_ = -1;
				}
			}
		}
	}
}


