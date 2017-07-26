
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
			Message.set_visible(false);
			
			id_swipe_view.pop();			
		}
	}

	// Right
	id_right_icon
	{
		text: fa_delete;
	}
	id_right_mousearea
	{
		onClicked:
		{
			Message.set_visible(false);

			Message.clear();			
			Envelope.restart();
			id_swipe_view.pop();
		}
	}
}
