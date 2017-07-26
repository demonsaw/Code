
import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.0

import "delegate"
import "header"
import "setting"
import "colorpicker/ColorUtils.js" as ColorUtils

Page
{	
	id: settings_root;

	// Variables
	property vector4d colorRGBA: ColorUtils.hsva2rgba(Client.get_color_hsva());
	property int current_index: 0;
    property bool setting_open: false;

    // For scaling (hacks)
    readonly property int row_size: screen_height > screen_width? screen_height / 15: screen_height / 10;
    readonly property int title_size: screen_height > screen_width? screen_height / 30: screen_height / 15;
    readonly property int icon_size: screen_height > screen_width?  screen_height / 24 : screen_height / 18;
    readonly property int ball_size: screen_height > screen_width? screen_height / 19 : screen_height / 14;
    readonly property int small_icon_size: screen_height > screen_width? screen_height / 32 : screen_height / 25;

	// Signals
	signal loadSetting(var qml);
	signal unloadSetting();
	signal updateClientText();
	signal open();
    signal close();

	// Open
	onOpen:
	{
		id_swipe_view.currentIndex = page_setting;
	}

	// Close
	onClose:
	{
		id_swipe_view.currentIndex = page_chat;
	}

	// Load setting
	onLoadSetting:
	{
		if (!setting_open)
		{
			id_swipe_view.push(qml);
			setting_open = true;
		}
	}

	// Unload setting
	onUnloadSetting:
	{
		if(adding_client)
		{
			adding_client = false;
			wizard_active = false;
			App.back();
		}
		id_swipe_view.pop();
		setting_open = false;
	}

	// Header
	header: SettingsHeader
	{
		id: id_header;
		anchors.left: parent.left;
		anchors.top: parent.top;
		height: header_size;
		width: parent.width;
	}

	// Body
	Rectangle
	{
		id: id_body;
		anchors.fill: parent;
		color: white;

		// Settings Listview
		ListView
		{
			id: settings_view;
            height: row_size * 4.5;
			anchors
			{
				left: parent.left;
				right: parent.right;
				top: parent.top;
			}
			boundsBehavior: Flickable.StopAtBounds;
			MouseArea
			{
				anchors.fill: parent;
				propagateComposedEvents: true;
				onPressed: mouse.accepted = false;
				onReleased: mouse.accepted = false;
				onClicked:
				{
					mouse.accepted = false;
				}
			}

			// Client Configuration
			SettingDelegate
			{
                id: id_client;
                height: row_size;
				setting_index: 0;

				setting_text_icon
				{
					text: fa_user;
				}

				setting_text
				{
					text: "Client";
				}
			}

			// Group Settings
			SettingDelegate
			{
                id: id_group;
				setting_index: 1;
                height: row_size;
				anchors.top: id_client.bottom;

				setting_text_icon
				{
					text: fa_shield;
				}

				setting_text
				{
					text: "Security";
				}

			}

			// Router Settings
			SettingDelegate
			{
                id: id_router;
				setting_index: 2;
                height: row_size;
				anchors.top: id_group.bottom;

				setting_text_icon
				{
					text: fa_server;
				}

				setting_text
				{
					text: "Network";
				}
			}

			// System Settings
			SettingDelegate
			{
                id: id_system;
				setting_index: 3;
                height: row_size;
				anchors.top: id_router.bottom;

				setting_text_icon
				{
					text: fa_wrench;
				}

				setting_text
				{
					text: "System";
				}
			}
		}

		// Client List
		ClientList
		{
            id: id_client_list;
            height: row_size * Client.size;
			anchors
			{
				left: parent.left;
				right: parent.right;
				top: settings_view.bottom;
			}
			width: parent.width;
		}

		// Add
		ListView
		{
            id: add_view;
            height: row_size;
			anchors
			{
				top: id_client_list.bottom;
				left: parent.left;
				right: parent.right;
			}
			boundsBehavior: Flickable.StopAtBounds;
			MouseArea
			{
				anchors.fill: parent;
				propagateComposedEvents: true;
				onPressed: mouse.accepted = false;
				onReleased: mouse.accepted = false;
				onClicked:
				{
					mouse.accepted = false;
				}
			}

			SettingDelegate
			{
				id: id_add;
				setting_index: 4;
				anchors.top: parent.top;
				enabled: Client.size < Client.max && !adding_client;
                height: row_size;

				setting_text_icon
				{
					color: enabled ? grey : light_grey;
					text: fa_add;
				}

				setting_text
				{
					color: enabled ? grey : light_grey;
					text: "Add";
				}
			}
		}
	}

	// Background
	Image
	{
		id: enigma_watermark;
		anchors
		{
			left: parent.left;
			right: parent.right;
			bottom: parent.bottom;
		}
		fillMode: Image.PreserveAspectFit;
		height: Math.min(parent.height * 0.75, parent.width * 0.75);
		opacity: watermark_opacity;
		source: "qrc:/logo_title.svg";
	}

	// Footer
	footer: Rectangle
	{
		id: settings_footer
		color: grey;
        height: header_size;
		width: parent.width;

		// Debug
		Button
		{
			id: id_debug;
			anchors
			{
				left: parent.left;
				verticalCenter: parent.verticalCenter;
			}
			height: icon_size;
			width: icon_size_2;

			background: Rectangle
			{
				color: grey;
			}

			contentItem: Text
			{
				anchors.left:parent.left
				anchors.right: parent.right;
				color: id_debug.pressed ? selected : unselected;
				font.family: "FontAwesome";
                font.pixelSize: id_root.icon_size;
                height: header_size;
				horizontalAlignment: Text.AlignHCenter;
				text: fa_debug;
				verticalAlignment: Text.AlignVCenter;
			}

			onClicked:
			{
				loadSetting("qrc:/qml/Debug.qml");
			}
		}

		// Exit Button
		Button
		{
			id: id_exit;
			anchors
			{
				horizontalCenter: parent.horizontalCenter;
				verticalCenter: parent.verticalCenter;
			}
			height: icon_size;
			width: icon_size_2;

			background: Rectangle
			{
				color: grey;
			}

			contentItem: Text
			{
				anchors.left:parent.left
				anchors.right: parent.right;
				color: id_exit.pressed ? selected : unselected;
				font.family: "FontAwesome";
                font.pixelSize: id_root.icon_size;
                height: header_size;
				horizontalAlignment: Text.AlignHCenter;
				text: fa_exit;
				verticalAlignment: Text.AlignVCenter;
			}

			onClicked:
			{
				id_root.visible = false;
				App.shutdown();
			}
		}

		// About
		Button
		{
			id: id_about;
			anchors
			{
				right: parent.right;
				verticalCenter: parent.verticalCenter;
			}
			height: icon_size;
			width: icon_size_2;

			background: Rectangle
			{
				color: grey;
			}

			contentItem: Text
			{
				anchors.left:parent.left
				anchors.right: parent.right;
				color: id_about.pressed ? selected : unselected;
				font.family: "FontAwesome";
                font.pixelSize: id_root.icon_size;
				height: header_size;
				horizontalAlignment: Text.AlignHCenter;
				text: fa_about;
				verticalAlignment: Text.AlignVCenter;
			}

			onClicked:
			{
				loadSetting("qrc:/qml/About.qml");
			}
		}
	}
}
