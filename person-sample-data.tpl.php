<?php

/**
 * @file
 * Example tpl file for theming a single person-specific theme
 *
 * Available variables:
 * - $status: The variable to theme (while only show if you tick status)
 * 
 * Helper variables:
 * - $person: The person object this status is derived from
 */
?>

<div class="person-status">
  <?php print '<strong>Person Sample Data:</strong> ' . $person_sample_data = ($person_sample_data) ? 'Switch On' : 'Switch Off' ?>
</div>