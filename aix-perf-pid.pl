#!/usr/bin/perl

use Getopt::Std;

getopt('urtp');

unless ($opt_r && $opt_t && $opt_p){
    print "Usage: $0 [ -u user] -r sample_count -t sleep_time -p pid\n";
    exit(0);
}

my $i;
my @proc = ($opt_p);

for ($i = 0; $i < $opt_r; $i++){
    foreach my $pid (@proc){
        my $command = "/usr/bin/procstack $pid";
        print `$command 2>/dev/null`;
    }
    select(undef, undef, undef, $opt_t);
}