 function generateCommand(checkbox){
	// alert($(checkbox).attr('value'));
	// alert($(checkbox).attr('name'));
	retrieveSwitchAndGenerateCommand(checkbox);
  }
  

  

function getNodeDetail(nodeId, switch_id, switchStatus) {
// alert('AJAX' + switchStatus);
$.ajax({
	url : 'get_node.php',
	type : 'POST',
	data: {node_id : nodeId},
	dataType : 'html',
	success : function(nodeDetails) {
		// alert(' getNode  success');
		sendSwitchCommand(nodeDetails,switch_id, switchStatus);
	},
	error : function(data) {
		alert('failure');
	}
});
}
	

//Switch ON OFF status
// as - on - 24947
//ad - off - 24932
// args for command - rf, switch id, node id master id

function sendSwitchCommand(nodeDetail,switch_id, switchStatus) {
// alert('sendSwitchCommand ' +	nodeDetail);
var str = nodeDetail.split(" ");
var sw_status = (switchStatus.toString().toUpperCase().trim().indexOf("ON") != -1) ? 24932 : 24947;
// alert('switchStatus dfinal'+ sw_status);
// alert('node details'+ str[0] + "  "+ str[1]);
var randomnumber=Math.floor(Math.random()*111111);
$.ajax({
	url : 'add_cmd.php',
	type : 'POST',
	data: {cmd: sw_status, req: randomnumber , 'arg1': str[0], 'arg2': switch_id ,'arg3': str[1],'arg4': str[2], 'uname': 'test'},
	dataType : 'html',
	success : function() {
		// alert('success');
	},
	error : function() {
		alert('failure');
	}
});
}

function retrieveSwitchAndGenerateCommand(checkbox) {
// alert('findSwitchStatus' + $(checkbox).attr('name'));
$.ajax({
	url : 'get_switch.php',
	type : 'POST',
	data: {switch_id : $(checkbox).attr('name')},
	dataType : 'html',
	success : function(switchStatus) {
	// alert('switchStatus success ' + switchStatus);
		getNodeDetail($(checkbox).attr('value'), $(checkbox).attr('name'), switchStatus);
	},
	error : function(data) {
		alert('failure');
	}
});
}