
import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.0

import "../delegate"
import "../header"

// Notification settings
Page
{
	id: system_settings_root;

	// Signals
	signal save();

	onSave:
	{
		Client.restart();
		App.save();
        Toast.show("Settings saved");
	}

	Component.onCompleted:
	{
		system_checkbox.checked = Client.flash;
	}

	// Header
	header: SettingsItemHeader
	{
		anchors
		{
			top: parent.top;
			left: parent.left;
		}

		enabled: !wizard_active;
		visible: !wizard_active;
		height: wizard_active? 0 : header_size;
		width: parent.width;
	}
	ListView
	{
		anchors.fill: parent;
		boundsBehavior: Flickable.StopAtBounds;
		SystemSettingDelegate
		{
			id: id_notify;
			anchors
			{
				top: parent.top;
				left: parent.left;
				right: parent.right;
			}
			height: header_size;
			system_setting_icon
			{
				text: fa_bell_o;
				font.pixelSize: icon_size;
			}
			system_setting_text
			{
				text: "Notifications";
			}
			system_checkbox
			{
				id: system_checkbox;
				onCheckedChanged: Client.flash = system_checkbox.checked;
			}
		}
	}
}
