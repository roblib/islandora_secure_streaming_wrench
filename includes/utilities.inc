<?php

/**
 * Adds a token to a source url.
 * @param array $source
 * @return string
 */
function islandora_streaming_wrench_tokenize_rtmp($source, $token) {
  $url = parse_url($source['url']);
  $path = $url['path'];
  $path_parts = pathinfo($path);
  $filename = $path_parts['basename'];
  $application = $path_parts['dirname'];
  $source['url'] = $url['scheme'] . '://' . $url['host'] . ':' . $url['port'] .
      $application . "?t=$token/$filename";
  return $source;
}

function islandora_streaming_wrench_tokenize($source, $token) {
  $source['url'] = $source['url'] . "?t=$token";
  return $source;
}

/**
 * Creates a token for use with Wowza's Wrench plugin
 * @global object $user
 *   A drupal user object
 * @return mixed
 *   Returns a token on success, NULL on failure.
 */
function islandora_streaming_wrench_create_token() {
  $token = uniqid();
  $ip = ip_address();
  global $user;  
  if(empty($user->name)){
    $user->name = 'anonymous user';
  }
  $db_entry = array(
    'ip' => $ip,
    'token' => $token,
    'username' => $user->name,
    'ts' => date("Y-m-d H:i:s"),
  );
  try {
    $test = drupal_write_record('islandora_streaming_tokens', $db_entry);
  } catch (Exception $e){
    drupal_set_message(t("Error creating secure token for video streaming. @details", array('@details' => $e->getMessage())), 'error');
  }
  return isset($test) ? $token : NULL;
}
