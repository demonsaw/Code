
import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.0

Header
{
	// Left
	id_left_icon
	{
		text: fa_back;
	}
	id_left_mousearea
	{
		onClicked:
		{
			id_setting.unloadSetting();
		}
	}

	// Right
	id_right_icon
	{
		text: adding_client? "" : fa_save;
	}
	id_right_mousearea
	{
		enabled: !adding_client;
		onClicked:
		{
			color: id_right_mousearea.pressed ? selected : unselected;

			switch (id_setting.current_index)
			{
				case 0:
				{
					client_settings_root.save();
					break;
				}
				case 1:
				{
					group_settings_root.save();
					break;
				}
				case 2:
				{
					session_settings_root.save();
					break;
				}
				case 3:
				{
					system_settings_root.save();
					break;
				}
			}
		}
	}
}
