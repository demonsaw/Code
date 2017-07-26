
import QtQml 2.2
import QtQuick 2.7
import QtQuick.Controls 2.0
import StatusBar 0.1
import QtQuick.Layouts 1.0

import "controls"

ApplicationWindow
{
	id: id_root;
	color: "transparent";
	height: 720;
	width: 405;
	title: qsTr("Enigma");
	visible: true;

	// Variables
    readonly property real ratio: App.get_ratio();
	//readonly property real font_ratio: App.get_font_ratio();
    readonly property string copyright: String.fromCharCode(0xA9) + " 2016 Eijah LLC.  All Rights Reserved.";
    readonly property string version: "Version 1.0.0 - February 6, 2017"

	readonly property string fa_add: "\uf067";
	readonly property string fa_about: "\uf128";
	readonly property string fa_arrow_down: "\uf062";
	readonly property string fa_arrow_left: "\uf060";
	readonly property string fa_arrow_right: "\uf061";
	readonly property string fa_arrow_up: "\uf063";
	readonly property string fa_back: "\uf060";
	readonly property string fa_ban: "\uf05e";
	readonly property string fa_bell_o: "\uf0a2";
	readonly property string fa_bolt: "\uf0e7";
	readonly property string fa_chat: "\uf0e5";
	readonly property string fa_clock_o: "\uf017";
	readonly property string fa_cog: "\uf013";
	readonly property string fa_delete: "\uf00d";
	readonly property string fa_ellipsis: "\uf142";
	readonly property string fa_debug: "\uf188";
	readonly property string fa_exit: "\uf011";
	readonly property string fa_file_image_o: "\uf1c5";
	readonly property string fa_menu: "\uf0c9";
	readonly property string fa_message: "\uf003";
	readonly property string fa_mobile: "\uf10b";
	readonly property string fa_next: "\uf061";
	readonly property string fa_prompt: "\uf105";
	readonly property string fa_refresh: "\uf021";
	readonly property string fa_save: "\uf0c7";	
	readonly property string fa_server: "\uf233";
	readonly property string fa_settings: "\uf013";
	readonly property string fa_shield: "\uf132";
	readonly property string fa_user: "\uf2c0";
	readonly property string fa_users: "\uf0c0";
	readonly property string fa_volume_up: "\uf028";
    readonly property string fa_wrench: "\uf0ad";

	readonly property string black: "#232320";
	readonly property string demonsaw: "#52C175";
	readonly property string green: "#09b24a";
	readonly property string grey: "#484840";
	readonly property string mid_grey: Qt.rgba(72/255, 72/255, 64/255, 0.5);
	readonly property string light_grey: "#ccd6dd";
	readonly property string lighter_grey: Qt.rgba(204/255, 214/255, 221/255, 0.75);
	readonly property string medium_grey: "#d0d0d0";
	readonly property string orange: "#b24a09";
	readonly property string red: "#b2091c";
	readonly property string yellow: "#b29f09";
	readonly property string white: "#ffffff";
	readonly property string blue: "#0971b2";
	readonly property string dark_blue_10: "#005899";
	readonly property string dark_blue_20: "#003E7F";
	readonly property string dark_blue_30: "#002566";
	readonly property string dark_blue_50: "#000033";
	readonly property string light_blue: "#56BEFF";
	readonly property string blue_light_50: "#88F0FF";
	readonly property string blue_light_60: "#BCFFFF";
	readonly property string blue_light_70: "#D5FFFF";
	readonly property string selected: light_blue;
	readonly property string soft_shadow: "#40000000";
	readonly property string softest_shadow: "#20000000";
	readonly property string unselected: white;
	readonly property string border_color: "#e0e0e0";

	readonly property string watermark_color: blue;
	readonly property double watermark_opacity: 0.05;
	readonly property double size_opacity: 0.10;

	readonly property int page_setting: 0;
	readonly property int page_chat: 1;
	readonly property int page_group: 2;
    readonly property int page_loaded: 3;
	
	readonly property int font_size: ratio * 48;
    readonly property double scale_ratio: screen_height / 720;
	readonly property int font_size_2: font_size * 2;
	readonly property int font_size_3: font_size * 3;
	readonly property int font_size_4: font_size * 4;
	readonly property int font_size_small: font_size * .75

	readonly property int background_size: Math.min(width / 2, height / 2);	
	readonly property int color_size: (font_size >> 1) & (~1);
	readonly property int cursor_size: (font_size * 2.50) & (~1);

	readonly property int header_size: (font_size * 2.60) & (~1);
	readonly property int header_size_2: header_size * 2
	readonly property int header_size_3: header_size * 3;
	readonly property int header_size_4: header_size * 4;
	readonly property int header_size_5: header_size * 5;

	readonly property int footer_size: header_size;
	readonly property int footer_size_2: footer_size * 2;
	readonly property int footer_size_3: footer_size * 3;
	readonly property int footer_size_4: footer_size * 4;
	readonly property int footer_size_5: footer_size * 5;

	readonly property int padding_size: font_size / 8;
	readonly property int padding_size_1: padding_size;
	readonly property int padding_size_2: padding_size * 2;
	readonly property int padding_size_3: padding_size * 3;
	readonly property int padding_size_4: padding_size * 4;
	
	readonly property int line_size: font_size / 12;
    readonly property int max_vertical_velocity: height * 12;
	readonly property int max_horizontal_velocity: width * 7.5;
	readonly property int press_velocity: 10;

	readonly property int button_height: combo_box_height * 0.90;
	readonly property int hamburger_icon_size: (icon_size * 1.1) & (~1);
	readonly property int icon_size: (font_size * 1.70) & (~1);
	readonly property int icon_size_2: icon_size * 2;
	readonly property int icon_size_3: icon_size * 3;
	readonly property int icon_size_4: icon_size * 4;

	readonly property int ball_size: (header_size * 0.85) & (~1);
	readonly property int ball_padding_size: (header_size * 0.15) & (~1);
	readonly property int ball_highlight_size: (header_size * 0.12) & (~1);

	readonly property int menu_height: font_size * 4;
	readonly property int menu_width: menu_height * 2.5;
	readonly property int small_icon_size: font_size * 1.25;
	readonly property int title_size: (font_size * 1.35) & (~1);
	readonly property int about_text_size: (font_size * 1.5) & (~1);

    readonly property int screen_height: height;
    readonly property int screen_width: width;
    readonly property int combo_box_height: height / 15;
	readonly property int combo_box_width: combo_box_height * 4;
	readonly property int box_font_size: Math.min(combo_box_height / 2, font_size);
    readonly property int margin_height: height / 48;
	readonly property int margin_height_color: font_size * 4;
	readonly property int margin_width: font_size * 0.75;

	property string selected_color: Client.color;
	property string client_text_color: white;

	// Boolean (Status Flags)
	property bool adding_client: false;
	property bool pop_right: true;
	property bool wizard_active: false;

	// Components
	property alias id_chat: id_chat;
	property alias id_clients: id_clients;
	property alias id_stack_view: id_stack_view;
	property alias id_status_bar: id_status_bar;
	property alias id_swipe_view: id_swipe_view;

	signal pop();
	signal popLeft();
	signal push(var qml);

	// Loaders
	FontLoader
	{
		source: "qrc:/font/awesome.ttf";
	}

	onClosing:
	{
		if(adding_client)
		{
			close.accepted = false;
			adding_client = false;
			wizard_active = false;
			App.back();
			id_swipe_view.pop();
		}
		else if(wizard_active)
		{
			close.accepted = true;
			return;
		}
		else if(id_swipe_view.loaded)
		{
			close.accepted = false;
			id_swipe_view.pop();
		}
		else if(id_swipe_view.currentIndex != page_chat)
		{
			close.accepted = false;
			id_swipe_view.currentIndex = page_chat;
		}
		else
		{
           close.accepted = false;
           Minimize.minimize_activity();
		}

		// For Back button on setting
		if(id_setting.setting_open)
			id_setting.setting_open = false;
	}

	Component.onCompleted:
	{
		// Load wizard on startup if no clients
		if (Client.empty())
		{
			wizard_active = true;
			id_stack_view.push("qrc:/qml/Wizard.qml");
		}
		id_splash.start();
	}

	Component.onDestruction:
	{
		id_root.visible = false;
		App.shutdown();
	}

    Connections
    {
        target: App;
        onNotification_clicked:
        {
            if (!adding_client && !wizard_active)
            {
                if (!id_swipe_view.loaded)
                {
                    id_swipe_view.currentIndex = page_group;
                }
                else
                {
                    id_swipe_view.openGroup();
                }

				id_chat.inactivate();
            }
        }
    }

	Splash
	{
		id: id_splash
		z:4;
		anchors.fill: parent;
	}

	StatusBar
	{
		id: id_status_bar
		color: Client.status;
	}

	StackView
	{
		id: id_stack_view
		anchors.fill:parent
		initialItem: id_swipe_view

		// Signals
		signal popRight();
		signal popLeft();

		onPopRight:
		{
			pop_right = true;
			pop();
		}
		onPopLeft:
		{
			pop_right = false;
			pop();
			pop_right = true // default value
		}
		// Animation Enter
		popEnter: Transition
		{
			XAnimator
			{
				from: (pop_right ? -1 : 1) * -id_stack_view.width
				to: 0
				duration: 400
				easing.type: Easing.OutCubic
			}
		}

		// Animation Exit
		popExit: Transition
		{
			XAnimator
			{
				from: 0
				to: (pop_right? -1 : 1) * id_stack_view.width
				duration: 400
				easing.type: Easing.OutCubic
			}
		}
	}

    CustomSwipeView
	{
		id: id_swipe_view

		Settings
		{
			id: id_setting;
		}

		Chat
		{
			id: id_chat;
		}

		Group
		{
			id: id_clients;
		}
    }
}
