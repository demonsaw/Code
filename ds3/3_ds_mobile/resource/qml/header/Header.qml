
import QtQuick 2.7
import QtQuick.Controls 2.0
import QtGraphicalEffects 1.0
import QtQuick.Layouts 1.0


Item
{
    // Variables
    property alias id_header: id_header;
    property alias id_center_text: id_center_text;
    property alias id_left_icon: id_left_icon;
    property alias id_left_mousearea: id_left_mousearea;
    property alias id_right_icon: id_right_icon;
    property alias id_right_mousearea: id_right_mousearea;
	
    // lil hack
    property int title_size: id_root.title_size;
    property int icon_size: id_root.icon_size;


    Rectangle
    {
        id: id_header
        anchors
        {
            top: parent.top;
            left: parent.left;
            right: parent.right;
        }
        color: blue;
        height: header_size;
        width: parent.width;
        layer.enabled: true;
        layer.effect: DropShadow
        {
            transparentBorder: true;
        }

        Row
        {
            anchors.fill: parent;

            Rectangle
            {
                id: id_left;
                anchors.verticalCenter: parent.verticalCenter;
                color: "transparent";
                height: parent.height;
                width: icon_size_2;

                Text
                {
                    id: id_left_icon
                    anchors.horizontalCenter: parent.horizontalCenter;
                    color: id_left_mousearea.pressed ? selected : unselected;
                    font.family: "FontAwesome";
                    font.pixelSize: icon_size;
                    height: parent.height;
                    text: "";
                    verticalAlignment: Text.AlignVCenter;

                    // Drop Shadow
                    layer.enabled: text != "";
                    layer.effect: DropShadow
                    {
                        transparentBorder: true;
                        color: softest_shadow;
                    }
                }
                MouseArea
                {
                    id: id_left_mousearea;
                    anchors.fill: parent;
                    preventStealing: true;
                }
            }

            Rectangle
            {
                color: "transparent";
                height: parent.height;
                width: parent.width - (id_left.width + id_right.width);

                Text
                {
                    id: id_center_text
                    anchors.centerIn: parent;
                    color: white;
                    font.pixelSize: title_size;
                    height: parent.height
                    horizontalAlignment: Text.AlignHCenter;
                    text: Client.name;                    
                    textFormat: Text.PlainText;
					verticalAlignment: Text.AlignVCenter;
					width: parent.width;

                    // Drop Shadow
                    layer.enabled: text != "";
                    layer.effect: DropShadow
                    {
                        transparentBorder: true;
                        color: softest_shadow;
                    }
                }
            }

            Rectangle
            {
                id: id_right;
                anchors.verticalCenter: parent.verticalCenter;
                color: "transparent";
                height: parent.height;
                width: icon_size_2;

                Text
                {
                    id: id_right_icon
                    anchors.horizontalCenter: parent.horizontalCenter;
                    color: id_right_mousearea.pressed ? selected : unselected;
                    font.family: "FontAwesome";
                    font.pixelSize: icon_size;
                    height: parent.height;
                    text: "";
                    verticalAlignment: Text.AlignVCenter;

                    // Drop Shadow
                    layer.enabled: text != "";
                    layer.effect: DropShadow
                    {
                        transparentBorder: true;
                        color: softest_shadow;
                    }
                }
                MouseArea
                {
                    id: id_right_mousearea;
                    anchors.fill: parent;
                }
            }
        }
    }
}
