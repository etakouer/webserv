<?php
session_start();

if (isset($_POST['erase'])) {
	session_destroy();
	header('Location: index.php');
  	exit();
} else if (isset($_POST['name'])) {
	$_SESSION['name'] = htmlspecialchars($_POST['name']);
}else if (isset($_GET['name'])) {
	$_SESSION['name'] = htmlspecialchars($_GET['name']);
}if (!isset($_SESSION['name'])) {
	echo "Hello what is your name ?<br>";
	?>
	<html>
		<h2>GET :</h2>
		<form method="get" action="index.php" class="form-group">
			<input name="name" placeholder="Name" required><br /><br />
			<input type="submit" value="Submit"/>
		</form>

		<h2>POST : </h2>
		<form method="post" action="index.php" class="form-group">
			<input name="name" placeholder="Name" required><br /><br />
			<input type="submit" value="Submit"/>
		</form>
	</html>
	<?php
}
else
{
	if (!isset($_SESSION['count'])) {
	  $_SESSION['count'] = 0;
	} else {
	  $_SESSION['count']++;
	}
	
	if (isset($_SESSION['name'])) {
		echo "Hello " . $_SESSION['name'] . " !<br>";
	}
	else {
		echo "Hello you !<br>";
	}
	echo "Your page have been refreshed " , $_SESSION['count'] , " times<br>";
	echo "Here a var dump of " , '$_SESSION : ', "<br>";
	var_dump($_SESSION);
?>
		<form method="post" action="index.php" class="form-group">
			<input type="submit" value="Erase session" name="erase" />
		</form>
	</html>

<?php

}
