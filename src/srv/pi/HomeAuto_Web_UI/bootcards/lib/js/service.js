 function generateCommand(checkbox){
	alert($(checkbox).attr('value'));
	getNodeDetail($(checkbox).attr('value'));
  }
  


function getNodeDetail(nodeId) {
alert('AJAX');
$.ajax({
	url : 'get_node.php',
	type : 'POST',
	data: {node_id : nodeId},
	dataType : 'html',
	success : function(nodeDetails) {
		alert(' getNode  success');
		sendSwitchCommand(nodeDetails);
	},
	error : function(data) {
		alert('failure');
	}
});
}

function sendSwitchCommand(nodeDetail) {
alert('sendSwitchCommand ' +	nodeDetail);
var str = nodeDetail.split(" ");
alert( str);
$.ajax({
	url : 'add_cmd.php',
	type : 'POST',
	data: {cmd: 666, req: 6 , 'arg1': 6, 'arg2': 6,'arg3': 6,'arg4': 6, 'uname': 'test'},
	dataType : 'html',
	success : function() {
		alert('success');
	},
	error : function() {
		alert('failure');
	}
});
}

