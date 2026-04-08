#include <pcap.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>

#include "../includes/globals.h"

typedef struct {
    time_t startSec;
    int intervalSec;
    size_t bucketsCap;
    size_t bucketsLen;
    unsigned long long *bytesPerBucket;
} IndicatorState;

static pcap_t *captureHandle = NULL;

void indicatorHandleSigint(void) {
    if (captureHandle != NULL) {
        pcap_breakloop(captureHandle);
    }
}

static int ensureCapacity(IndicatorState *state, size_t idx) {
    if (idx < state->bucketsCap) {
        return 1;
    }

    size_t newCap = state->bucketsCap == 0 ? 64 : state->bucketsCap;
    while (newCap <= idx) {
        newCap *= 2;
    }

    unsigned long long *newData = realloc(state->bytesPerBucket, newCap * sizeof(unsigned long long));
    if (newData == NULL) {
        return 0;
    }

    for (size_t i = state->bucketsCap; i < newCap; i++) {
        newData[i] = 0ULL;
    }

    state->bytesPerBucket = newData;
    state->bucketsCap = newCap;
    return 1;
}

static void packetHandler(unsigned char *user, const struct pcap_pkthdr *header, const unsigned char *packet) {
    (void)packet;
    IndicatorState *state = (IndicatorState *)user;

    time_t sec = (time_t)header->ts.tv_sec;
    if (state->startSec == 0) {
        state->startSec = sec;
    }

    long long delta = (long long)(sec - state->startSec);
    if (delta < 0) {
        return;
    }

    size_t bucket = (size_t)(delta / state->intervalSec);
    if (!ensureCapacity(state, bucket)) {
        fprintf(stderr, "Failed to grow indicator buckets\n");
        stopRequested = 1;
        if (captureHandle != NULL) {
            pcap_breakloop(captureHandle);
        }
        return;
    }

    if (bucket + 1 > state->bucketsLen) {
        state->bucketsLen = bucket + 1;
    }
    state->bytesPerBucket[bucket] += (unsigned long long)header->caplen;
}

int runIndicatorMode(void) {
    if (indicatorIntervalSec <= 0) {
        printf("%sIndicator interval must be > 0%s\n", RED, RESET);
        return 1;
    }

    char errbuf[PCAP_ERRBUF_SIZE];
    captureHandle = pcap_open_live(interface, 65535, 1, 1000, errbuf);
    if (captureHandle == NULL) {
        printf("%spcap_open_live failed:%s %s\n", RED, RESET, errbuf);
        return 1;
    }

    if (strlen(pcapFilter) > 0) {
        struct bpf_program fp;
        if (pcap_compile(captureHandle, &fp, pcapFilter, 1, PCAP_NETMASK_UNKNOWN) < 0) {
            printf("%spcap_compile failed:%s %s\n", RED, RESET, pcap_geterr(captureHandle));
            pcap_close(captureHandle);
            captureHandle = NULL;
            return 1;
        }
        if (pcap_setfilter(captureHandle, &fp) < 0) {
            printf("%spcap_setfilter failed:%s %s\n", RED, RESET, pcap_geterr(captureHandle));
            pcap_freecode(&fp);
            pcap_close(captureHandle);
            captureHandle = NULL;
            return 1;
        }
        pcap_freecode(&fp);
    }

    IndicatorState state;
    memset(&state, 0, sizeof(state));
    state.intervalSec = indicatorIntervalSec;
    stopRequested = 0;

    printf("%sMode:%s INDICATOR\n", BLUE, RESET);
    printf("%sInterface:%s %s\n", BLUE, RESET, interface);
    printf("%sFilter:%s %s\n", BLUE, RESET, strlen(pcapFilter) > 0 ? pcapFilter : "<none>");
    printf("%sBucket interval:%s %d second(s)\n", BLUE, RESET, indicatorIntervalSec);
    if (indicatorDurationSec > 0) {
        printf("%sDuration:%s %d second(s)\n", BLUE, RESET, indicatorDurationSec);
    } else {
        printf("%sDuration:%s until Ctrl+C\n", BLUE, RESET);
    }

    if (indicatorDurationSec > 0) {
        time_t deadline = time(NULL) + indicatorDurationSec;
        while (!stopRequested && time(NULL) < deadline) {
            int rc = pcap_dispatch(captureHandle, -1, packetHandler, (unsigned char *)&state);
            if (rc == -1) {
                printf("%spcap_dispatch error:%s %s\n", RED, RESET, pcap_geterr(captureHandle));
                break;
            }
        }
    } else {
        int rc = pcap_loop(captureHandle, -1, packetHandler, (unsigned char *)&state);
        if (rc == -1 && !stopRequested) {
            printf("%spcap_loop error:%s %s\n", RED, RESET, pcap_geterr(captureHandle));
        }
    }

    pcap_close(captureHandle);
    captureHandle = NULL;

    FILE *out = fopen(indicatorOutputCsv, "w");
    if (out == NULL) {
        perror("fopen");
        free(state.bytesPerBucket);
        return 1;
    }

    fprintf(out, "bucket_start_sec,bytes\n");
    for (size_t i = 0; i < state.bucketsLen; i++) {
        fprintf(out, "%zu,%llu\n", i * (size_t)indicatorIntervalSec, state.bytesPerBucket[i]);
    }
    fclose(out);

    printf("%sSaved indicator CSV:%s %s\n", GREEN, RESET, indicatorOutputCsv);
    free(state.bytesPerBucket);
    return 0;
}

int indicatorListInterfaces(void) {
    char errbuf[PCAP_ERRBUF_SIZE];
    pcap_if_t *alldevs = NULL;
    if (pcap_findalldevs(&alldevs, errbuf) == -1) {
        printf("%sFailed to list interfaces:%s %s\n", RED, RESET, errbuf);
        return 0;
    }

    int idx = 1;
    for (pcap_if_t *d = alldevs; d != NULL; d = d->next) {
        printf("%s%i.Interface:%s %s", BLUE, idx, RESET, d->name);
        if (d->description != NULL) {
            printf(" (%s)", d->description);
        }
        printf("\n");
        idx++;
    }

    if (idx == 1) {
        printf("No interfaces found.\n");
    }

    pcap_freealldevs(alldevs);
    return 1;
}
